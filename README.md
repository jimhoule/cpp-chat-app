# C++ Chat App

Chat client and server built using C++.

## Stack

- C++17
- CMake
- Docker

## Project layout

| Directory | Contents |
|---|---|
| `core/` | Shared between client and server — socket layer, JSON wrapper, event types with their serializers and deserializers, models, logging, observer, layer stack |
| `server/` | Socket server, modules, handlers, services, repositories, middlewares |
| `client/` | GLFW/ImGui desktop app — layers, navigation, APIs, local stores |
| `gui/` | ImGui wrapper (`Gui`) used by the client layers |
| `vendor/` | Submodules (GLFW, GLAD, ImGui, glm, nlohmann_json, libsodium, stduuid, stb, epoll-shim) |
| `test/` | CTest suite (currently a placeholder) |

## Build and run

```bash
make build_debug                              # cmake -S . -B build -D CMAKE_BUILD_TYPE=Debug
make start_server
make start_client_local                       # single client, session in tmp/
make start_client_local CLIENT_NAME=client1   # named client, session in tmp/client1/
make start_all_local                          # tmux: server + two clients, separate sessions
```

Build types are constrained to `Debug` and `Release` in the top-level `CMakeLists.txt`; anything else is a configure error. The build type is baked into the client binary and decides where it stores data, so Debug and Release never share a session.

## Socket protocol

Both sides speak one JSON shape over a raw TCP socket:

```json
{ "name": "LOGGEDIN", "payload": { "sessionId": "...", "user": { ... } } }
```

`SocketEventName` travels as a **string**, not an enum position, so inserting a value in the middle can't silently change the meaning of the ones after it. Every event has a typed payload struct in `core/include/socket/`, with a serializer and/or a deserializer in the neighbouring directories — each side only builds the direction it uses, so the pairs are deliberately asymmetric.

Events are named as command/result pairs: `LOGIN`/`LOGGEDIN`, `REGISTER`/`REGISTERED`, `AUTHENTICATE`/`AUTHENTICATED`, `LOGOUT`/`LOGGEDOUT`. Broadcasts are their own names: `USER_ONLINE`, `USER_OFFLINE`, `MESSAGE_CREATED`.

Failures come back as a single `ERROR` event carrying a `SocketErrorCode` and the `failedEventName` of the request that failed, so the client can tell which request it answers.

## Server

Composition happens in `server/src/main.cpp`. Each **module** owns its loggers, service and handler, and registers its routes on the `SocketServer`:

```
SocketServer  →  route (middlewares + handler)  →  Service  →  Repository
```

- **Handlers** deserialize the payload, call a service, serialize the result, send it. They never contain domain rules.
- **Services** own the domain rules and take `Dto` structs as input, returning a `ServiceResult<Code, Data>` — a result code plus an optional payload. They never throw for expected failures.
- **Repositories** are behind interfaces (`ISessionsRepository`, `IUsersRepository`, …) with in-memory implementations today, so storage can be swapped without touching services.
- **Middlewares** run before a handler and stop the route by throwing. `RequireAuthentication` rejects unauthenticated sockets and populates `context.user`.
- **`HandleErrors`** is the single error sink: it catches `ExpectedException` (client at fault), `JsonException` (malformed payload) and anything else (server at fault), logs at the right level, and replies with an `ERROR` event.

Services never call their own public methods — shared logic goes into a private helper instead. `SessionsService::IsExpired` and `GenerateExpirationTime` are examples.

### Connections

`SocketServer` keeps two indexes that must agree:

- `m_connectionsMap` — socket → `Connection` (which holds an optional `User`)
- `m_userClientSocketsMap` — user id → sockets, for fan-out to someone on several devices

`BindConnectionUser` and `UnbindConnectionUser` are the only things allowed to touch them, and `BindConnectionUser` unbinds first. That makes rebinding safe, which matters because one TCP connection can be bound to several users over its life — the client keeps a single connection for the whole app run, so logging out and back in as someone else reuses the same socket.

`UnbindConnectionUser` is idempotent, so the logout handler, `CloseClientSocket` and `BindConnectionUser` can all call it without checking first.

## Client

`client/src/main.cpp` is wiring only — loggers, socket, GUI, stores, APIs, navigation, screens — then the render loop. It performs no actions.

- **Layers** are screens (`SplashLayer`, `LoginLayer`, `RegisterLayer`, `ChatLayer`) with an `OnAttach` / `OnUpdate` / `OnRender` / `OnDetach` lifecycle, held in a `LayerStack`.
- **Navigation** owns the layers for the app's lifetime and pushes them onto the stack.
- **APIs** (`AuthApi`, `MessagesApi`) are transport: they serialize and send, register socket handlers, and notify a `Subject<TEvent>` so layers can subscribe without knowing about sockets.
- **`SocketClient`** allows several handlers per event name. Each handler is registered with an owner (the API's `this`), and `Off` removes only that owner's handler.
- **Stores** are local persistence. `SessionStore` is the only one so far.

Layers follow one shape: call an API method, subscribe to the matching subject, navigate when it fires.

## Auth and sessions

### Flow

```
first run            SplashLayer → no stored session → LoginLayer
login / register     server creates a session → client saves the id → ChatLayer
later runs           SplashLayer → AUTHENTICATE with the stored id → AUTHENTICATED → ChatLayer
stale session        SplashLayer → AUTHENTICATE → ERROR → AuthApi clears the id → LoginLayer
logout               LOGOUT → server deletes the session and unbinds → LOGGEDOUT → LoginLayer
shutdown             connection drops, session record survives
```

A session lasts 7 days and is refreshed on every successful `AUTHENTICATE`, so an active user is never logged out. Sessions are in-memory on the server, so every server restart makes stored ids stale; during development that is the normal path, not an edge case.

### Where the session id lives

The server stores the session **record** (id, userId, createdAt, expiredAt). The client stores only the **id**, because a fresh TCP connection carries no identity — the server can't know who is connecting until the client presents something. Same split as a browser cookie.

On disk, as JSON, one file per build type:

```
~/Library/Application Support/Chat/Debug/session     macOS
~/.config/Chat/Debug/session                         Linux
%APPDATA%\Chat\Debug\session                         Windows
```

`LOCAL_DATA_DIRECTORY` overrides the directory, which is what lets `make start_all_local` run two clients as two different users on one machine.

The file is written with mode `0600` — the default would let every account on the machine read a credential that is enough to authenticate as the user.

## Design decisions

**One connection for the app's lifetime.** The client connects once at startup and disconnects at shutdown. Logout does not drop the socket — it ends the session and unbinds the connection server-side. This is why `BindConnectionUser` has to tolerate rebinding.

**Logout clears the stored session, shutdown does not.** Surviving shutdown is the entire reason the id is on disk. The two are different intentions: closing the app means "I'm done for now", logging out means "forget me".

**`AUTHENTICATED` carries only the user, no session id.** The client sent the id in the request, so echoing it back is redundant. `LOGGEDIN` and `REGISTERED` do carry it because they mint a new session.

**`Authenticate()` and `Logout()` take no parameters.** `Login` and `Register` take params because the user typed something. The other two need only the stored session id, which `AuthApi` can read itself — passing it in would make the calling layer a courier for a value it never uses. `HasStoredSession()` is the one query that leaks out, because `SplashLayer` has to branch on it.

**Logout on a missing session id is not an error.** The caller wanted the session gone and it is. More importantly, logout must still unbind the connection, which is true whether or not the id was found.

**Each API handles its own errors, there is no error API.** An API that must react to a failure registers `ERROR` on `SocketClient` itself and filters on `failedEventName` (the requests it sends) first, then on the code. Layers listen to their API's events, never to `ERROR`. A separate error API was only needed while `SocketClient` allowed one handler per event name.

**Any `AUTHENTICATE` failure leaves the splash screen, only a dead session clears the store.** `AuthApi` notifies `AuthenticateFailedEvent` for every failure so the splash screen can never hang, but clears the stored id only on `UNKNOWN_SESSION` / `EXPIRED_SESSION`. On `INTERNAL` the session may still be valid, so the next launch retries it. The store is cleared before the event is sent, so it is clean before the UI reacts.

**Services take `Dto` structs and return `ServiceResult`.** Expected failures are result codes, not exceptions. Only handlers throw, and only `ExpectedException`, which `HandleErrors` turns into a client-facing code.

**Stores are per build type.** A Debug and a Release build are different binaries with different baked-in constants, so they can't collide on one session file.

**JSON for the session file even though it holds one field.** Consistency with the settings store that will hold several.

## Known gaps

- `SocketClient::Send` and `Connect` ignore failures, which makes a dead server look like a hung client.
- `ConversationsModule` is never constructed in `server/src/main.cpp`, so its four routes don't exist.
- `ChatLayer` still builds its users, conversations and messages from hardcoded data.
- The user's password hash is serialized into four events, including the `USER_ONLINE` broadcast. The planned view types remove it.
- Expired and orphaned sessions are never deleted from the repository.
- `USER_OFFLINE` is only sent on logout, not on disconnect, so presence goes stale when a client closes.

See `NEXT_STEPS.txt`.
