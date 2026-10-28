# C++ Chat App

Chat client and server built using C++.

## Stack

- C++17
- CMake
- Docker

## Project layout

| Directory | Contents |
|---|---|
| `core/` | The chat platform's shared toolkit, used by the client, the server and any other app speaking this protocol — socket layer, JSON wrapper, event types with their serializers and deserializers, models, views, utils (strings, `Debouncer`), logging, observer, layer stack |
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

TCP is a byte stream, not a message stream: one `read()` can return half an event or several glued together. Every event is therefore sent as a **length-prefixed frame**, `[4-byte payload length, network byte order][payload]`, built and parsed by `core/socket/Framer`. Each socket has its own `Framer` (in each `Connection` on the server, in the single `m_connection` on the client) that buffers bytes across reads and hands back only complete payloads. A length above 10 MB means the stream is out of sync and the connection is closed.

`SocketEventName` travels as a **string**, not an enum position, so inserting a value in the middle can't silently change the meaning of the ones after it. Every event has a typed payload struct in `core/include/socket/`, with a serializer and/or a deserializer in the neighbouring directories — each side only builds the direction it uses, so the pairs are deliberately asymmetric.

Events are named as command/result pairs: `LOGIN`/`LOGGEDIN`, `REGISTER`/`REGISTERED`, `AUTHENTICATE`/`AUTHENTICATED`, `LOGOUT`/`LOGGEDOUT`, `OPEN_CONVERSATION`/`CONVERSATION_OPENED`, `CLOSE_CONVERSATION`/`CONVERSATION_CLOSED`, `FIND_ALL_OPEN_CONVERSATIONS_BY_USER_ID`/`ALL_OPEN_CONVERSATIONS_FOUND_BY_USER_ID`, `SEARCH_USERS`/`USERS_SEARCHED`. Broadcasts are their own names: `USER_ONLINE`, `USER_OFFLINE`, `MESSAGE_CREATED`.

Failures come back as a single `ERROR` event carrying a `SocketErrorCode` and the `failedEventName` of the request that failed, so the client can tell which request it answers.

## Server

Composition happens in `server/src/main.cpp`. Each **module** owns its loggers, service and handler, and registers its routes on the `SocketServer`:

```
SocketServer  →  route (middlewares + handler)  →  Service  →  Repository
```

- **Handlers** receive an `EventContext` (client socket, `serializedPayload`, and `user` once authenticated), deserialize the payload, call a service, serialize the result, send it. They never contain domain rules.
- **Services** own the domain rules and take `Dto` structs as input, returning a `ServiceResult<Code, Data>` — a result code plus an optional payload. They never throw for expected failures.
- **Views** are what services return: `Dto` in, `View` out. Repositories return models (`User`, `Conversation`, `Message`), every public service method returns views (`UserView`, `ConversationView`, `MessageView`), so handlers and serializers never see a password. Each view has its JSON conversion in `core/json` (`UserJson`, `ConversationJson`, `MessageJson`), shared by every serializer and deserializer that carries it.
- **Repositories** are behind interfaces (`ISessionsRepository`, `IUsersRepository`, …) with in-memory implementations today, so storage can be swapped without touching services.
- **Middlewares** run before a handler and stop the route by throwing. `RequireAuthentication` rejects unauthenticated sockets and populates `context.user`.
- **`HandleErrors`** is the single error sink: it catches `ExpectedException` (client at fault), `JsonException` (malformed payload) and anything else (server at fault), logs at the right level, and replies with an `ERROR` event.

Services never call their own public methods — shared logic goes into a private helper instead. `SessionsService::IsExpired` and `GenerateExpirationTime` are examples. Calling *another* service's public methods is fine, and returns views.

### Connections

`SocketServer` keeps two indexes that must agree:

- `m_connectionsMap` — socket → `Connection` (which holds an optional `UserView`, the socket's `Framer` and its write buffer)
- `m_userClientSocketsMap` — user id → sockets, for fan-out to someone on several devices

`BindConnectionUser` and `UnbindConnectionUser` are the only things allowed to touch them, and `BindConnectionUser` unbinds first. That makes rebinding safe, which matters because one TCP connection can be bound to several users over its life — the client keeps a single connection for the whole app run, so logging out and back in as someone else reuses the same socket.

`UnbindConnectionUser` is idempotent, so the logout handler, `CloseClientSocket` and `BindConnectionUser` can all call it without checking first.

## Client

`client/src/main.cpp` is wiring only — loggers, socket, GUI, stores, APIs, navigation, screens — then the render loop. It performs no actions.

- **Layers** are screens (`SplashLayer`, `LoginLayer`, `RegisterLayer`, `ChatLayer`) with an `OnAttach` / `OnUpdate` / `OnRender` / `OnDetach` lifecycle, held in a `LayerStack`.
- **Navigation** owns the layers for the app's lifetime and pushes them onto the stack.
- **APIs** (`AuthApi`, `ConversationsApi`, `MessagesApi`, `UsersApi`) are transport: they serialize and send, register socket handlers, and notify a `Subject<TEvent>` so layers can subscribe without knowing about sockets.
- **`SocketClient`** allows several handlers per event name. Each handler is registered with an owner (the API's `this`), and `Off` removes only that owner's handler. Handlers receive a `SocketClient::EventContext`, the same shape as the server's (only `serializedPayload` today), so new per-event data can be added without changing every handler's signature.
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

**`models/User.h` stays in `core/`.** `core/` is the chat platform's shared toolkit, not a generic library: anything speaking this protocol (the client, this server, a second server) builds on it, so the protocol enum, the models and the views all belong there. The price is that keeping passwords off the wire is a convention, not something the compiler enforces: only a users repository or `UsersService` touches `User`, and the password hash never leaves `UsersService` (`VerifyCredentials` checks it internally). `grep -rn "models/User.h"` should only hit the users repository.

**Conversation views embed their participants as `UserView`s, fetched in one batch.** The server resolves user info rather than the client, since a client round trip costs far more than a lookup inside the server. `ConversationsService` collects the distinct participant ids of every conversation it returns, calls `UsersService::FindByIds` once (an `unordered_set` of ids, one repository query), and assembles the views from an id → `UserView` map, so there is no per-conversation lookup (N+1). A participant whose user no longer exists is skipped with a warning. Messages carry only `senderId`, resolved against the conversation's participants.

**User search is server-side.** `SEARCH_USERS` sends the typed text; the server matches it case-insensitively against email, first and last name and answers `USERS_SEARCHED` with `UserView`s. An empty search returns nothing rather than every user. The caller is not excluded, since you can message yourself. The client debounces the input (300 ms, `core/utils/Debouncer`, polled in `ChatLayer::OnUpdate` since the render loop has no timers), so one request goes out once typing stops instead of one per keystroke.

**Opening a conversation sends only the other people.** The client sends the user ids it picked; the server adds the initiator from the authenticated connection, never from the payload, then sorts and deduplicates them. Picking yourself therefore gives a one-participant conversation, shown as "<first name> (you)". `Open` is idempotent: the same set of users always returns the same conversation, so the client replaces it if it is already listed instead of adding a duplicate, then selects it. The sidebar only changes when the server answers (`CONVERSATION_OPENED` / `CONVERSATION_REOPENED` / `CONVERSATION_CLOSED`), never optimistically.

**Sending a message reaches every participant and reopens the conversation.** `MessagesService::Create` first checks that the sender is a user of the conversation. An unknown conversation and a non-member give the same `NOT_FOUND`, so a client cannot probe which conversation ids exist. Then it validates the text, stores the message, and reopens the conversation for every user who had closed it (`OpenForAllUsers`), returning the message, the conversation and the ids of the users it reopened for. The handler sends those users `CONVERSATION_REOPENED` first, so their client lists the conversation before the message arrives, then sends `MESSAGE_CREATED` to every participant on all their sockets, the sender included, so their other devices get it too. A user with no socket is offline and is skipped quietly. The client shows a message only when the server sends it back, never optimistically.

**Reopened is its own event, not `CONVERSATION_OPENED`.** `CONVERSATION_OPENED` answers the client's own `OPEN_CONVERSATION`, so the client adds the conversation and selects it. `CONVERSATION_REOPENED` means someone else's message brought it back, so the client adds it without taking the focus. Both carry a `ConversationView`, like `CONVERSATION_OPENED` and `CONVERSATION_CLOSED` already do. Separate names keep the client stateless (no "did I ask for this?" flag), let future listeners such as notifications or unread counts subscribe to reopens only, and let the payloads grow apart. Only users who had the conversation closed get it, so an active chat does not resend the whole conversation with every message.

**Length prefix framing, not a delimiter.** It is the usual choice for C++ servers and binary protocols, the size is known before reading, and it keeps working if JSON is ever replaced by a binary format, where a delimiter byte could appear inside the payload. `Framer` takes and returns `std::string` as a byte container (binary-safe when built with an explicit size), so a binary serializer would need no `Framer` change. `Framer` throws and the socket class logs and closes, so `Framer` needs no logger.

**Both sides drain the socket on every read.** Client sockets on the server are edge-triggered (`EPOLLET`), so epoll does not notify again for bytes left in the socket: `ReadClientSocket` reads until `EAGAIN`. The client does the same in `Read()`, called by its once-per-render-frame `Update()`, otherwise a large payload would take many render frames to arrive. All complete frames are extracted first, then dispatched in order.

**Both sides keep a write buffer, neither waits on `send()`.** A non-blocking `send()` can take only part of a frame, and dropping the rest would corrupt the framed stream. Each `Connection` keeps the unsent bytes in `writeBuffer` and `SendWriteBuffer` sends what it can. On the server, client sockets are registered for `EPOLLOUT` too, and epoll reports when a full socket has room again. On the client, `Update()` retries once per render frame. The client does not block until the socket has room, since that would freeze the render loop. On macOS `send()` during a non-blocking connect returns `ENOTCONN`, so the client keeps the bytes and retries, the same as for `EAGAIN`. The server never closes inside `SendTo`, because `SendAll` loops over `m_connectionsMap`: on a send error it drops the buffer and lets epoll's `EPOLLERR` / `EPOLLHUP` close the socket. SIGPIPE is disabled (`MSG_NOSIGNAL` on Linux, `SO_NOSIGPIPE` on macOS) so a peer that is gone returns an error instead of killing the process.

**Stores are per build type.** A Debug and a Release build are different binaries with different baked-in constants, so they can't collide on one session file.

**JSON for the session file even though it holds one field.** Consistency with the settings store that will hold several.

## Known gaps

- `SocketClient::Connect` and `SocketServer`'s setup call `exit()` instead of reporting the failure.
- The write buffers have no cap: a peer that never reads makes them grow without limit.
- Message history is not loaded: a conversation only shows the messages sent during this run. Group conversations exist on the server (up to 9 users) but the client can only start one-on-one conversations, one search result at a time.
- Expired and orphaned sessions are never deleted from the repository.
- Presence is login-based and wrong with several devices: logging out on one sends `USER_OFFLINE` while another is still connected, and closing the app sends nothing. Planned: presence based on authenticated connections (online when a user's first socket is bound to them at login, offline when their last one is unbound), not on raw TCP connections, which exist before login.

See `NEXT_STEPS.txt`.
