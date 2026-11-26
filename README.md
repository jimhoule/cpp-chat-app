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
| `gui/` | ImGui wrapper (`Gui`) and GUI helpers (`ScrollAnchor`) used by the client layers |
| `vendor/` | Submodules (GLFW, GLAD, ImGui, glm, nlohmann_json, libsodium, stduuid, stb, epoll-shim) |
| `test/` | CTest suite (currently a placeholder) |
| `dashboard/` | Planned, not created yet: standalone Unreal Slate telemetry dashboard (see "Planned: telemetry dashboard") |

## Build and run

```bash
make build_debug                              # cmake -S . -B build -D CMAKE_BUILD_TYPE=Debug
make start_server
make start_client_local                       # single client, session in tmp/
make start_client_local CLIENT_NAME=client1   # named client, session in tmp/client1/
make start_all_local                          # tmux: server + two clients, separate sessions
```

Build types are constrained to `Debug` and `Release` in the top-level `CMakeLists.txt`; anything else is a configure error. The build type is baked into the client binary and decides where it stores data, so Debug and Release never share a session.

Source files are collected with `file(GLOB ...)`, which CMake evaluates only when it configures. After adding a `.cpp` file, re-run `make build_debug` (or `cmake ..` in `build/`) before building, otherwise the link fails with "Undefined symbols".

## Socket protocol

Both sides speak one JSON shape over a raw TCP socket:

```json
{ "name": "LOGGEDIN", "payload": { "sessionId": "...", "user": { ... } } }
```

TCP is a byte stream, not a message stream: one `read()` can return half an event or several glued together. Every event is therefore sent as a **length-prefixed frame**, `[4-byte payload length, network byte order][payload]`, built and parsed by `core/socket/Framer`. Each socket has its own `Framer` (in each `Connection` on the server, in the single `m_connection` on the client) that buffers bytes across reads and hands back only complete payloads. A length above 10 MB means the stream is out of sync and the connection is closed.

`SocketEventName` travels as a **string**, not an enum position, so inserting a value in the middle can't silently change the meaning of the ones after it. Every event has a typed payload struct in `core/include/socket/`, with a serializer and/or a deserializer in the neighbouring directories — each side only builds the direction it uses, so the pairs are deliberately asymmetric.

Events are named as command/result pairs: `LOGIN`/`LOGGEDIN`, `REGISTER`/`REGISTERED`, `AUTHENTICATE`/`AUTHENTICATED`, `LOGOUT`/`LOGGEDOUT`, `OPEN_CONVERSATION`/`CONVERSATION_OPENED`, `CLOSE_CONVERSATION`/`CONVERSATION_CLOSED`, `FIND_ALL_OPEN_CONVERSATIONS_BY_USER_ID`/`ALL_OPEN_CONVERSATIONS_FOUND_BY_USER_ID`, `FIND_MESSAGES_PAGE_BY_CONVERSATION_ID`/`MESSAGES_PAGE_FOUND_BY_CONVERSATION_ID`, `FIND_ALL_OPEN_MESSAGES_READ_STATES_BY_USER_ID`/`ALL_OPEN_MESSAGES_READ_STATES_FOUND_BY_USER_ID`, `FIND_MESSAGES_READ_STATE_BY_CONVERSATION_ID`/`MESSAGES_READ_STATE_FOUND_BY_CONVERSATION_ID`, `MARK_MESSAGES_READ`/`MESSAGES_MARKED_READ`, `SEARCH_USERS`/`USERS_SEARCHED`. `MESSAGES_MARKED_READ` goes to every socket of the user, not only the one that asked, so their other devices update their badges. Broadcasts are their own names: `USER_ONLINE`, `USER_OFFLINE`, `MESSAGE_CREATED`.

Failures come back as a single `ERROR` event carrying a `SocketErrorCode` and the `failedEventName` of the request that failed, so the client can tell which request it answers.

## Server

Composition happens in `server/src/main.cpp`. Each **module** owns its loggers, service and handler, and registers its routes on the `SocketServer`:

```
SocketServer  →  route (middlewares + handler)  →  Service  →  Repository
```

- **Handlers** receive an `EventContext` (client socket, `serializedPayload`, and `userId` once authenticated), deserialize the payload, call a service, serialize the result, send it. They never contain domain rules.
- **Services** own the domain rules and take `Dto` structs as input, returning a `ServiceResult<Code, Data>` — a result code plus an optional payload. They never throw for expected failures.
- **Views** are what services return: `Dto` in, `View` out. Repositories return models (`User`, `Conversation`, `Message`), every public service method returns views (`UserView`, `ConversationView`, `MessageView`), so handlers and serializers never see a password. Each view has its JSON conversion in `core/json` (`UserJson`, `ConversationJson`, `MessageJson`), shared by every serializer and deserializer that carries it.
- **Repositories** are behind interfaces (`ISessionsRepository`, `IUsersRepository`, …) with in-memory implementations today, so storage can be swapped without touching services.
- **Middlewares** run before a handler and stop the route by throwing. `RequireAuthentication` rejects unauthenticated sockets and sets `context.userId`.
- **Handler binding.** Handlers are bound as an object and a method, the way the client's `Observer` binds `(*this, &ChatLayer::HandleX)`. A module registers `socketServer.On(SocketEventName::SEARCH_USERS, { RequireAuthentication(socketServer) }, m_usersHandler, &UsersHandler::HandleSearchUsers)`.
  - **How:** two `On` templates (with and without middlewares) wrap the object and method into an `EventHandler` and pass it to the private `AddRoute`. Routing, middlewares and dispatch didn't change. There is no public `std::function` overload, so every route is a named method.
  - **Why:** handlers are plain public `Handle*` methods. Stack traces name them, tests can call them directly, and the naming matches the client.
  - **Safety:** `AddRoute` refuses a second route for the same event (Error log, the first one stays) and rejects a null method.
  - **No owner and no `Off` on the server:** it stores one route per event name and never removes one. Routes live as long as the process, and at shutdown the modules are destroyed after the event loop has stopped.
- **`HandleErrors`** is the single error sink: it catches `ExpectedException` (client at fault), `JsonException` (malformed payload) and anything else (server at fault), logs at the right level, and replies with an `ERROR` event.

**Naming.** Every Find and Count method in a repository or service ends in `By<Key>`, where `Key` is the name of its main parameter. Other parameters stay out of the name: `MessagesService::FindPageByConversationId` takes `{ conversationId, userId, cursor }`. A module calls its own entity's id `id`, so it writes `ById` (`ConversationsService::FindById`, `FindLastReadMessagePositionById(id, userId)`). It calls another module's entity `<entity>Id` and writes `By<Entity>Id` (`MessagesService::FindReadStateByConversationId`). Other verbs take no `By` (`AdvanceLastReadMessagePosition`, `IsConversationUser`, `UpdateIsOpen`, `MarkRead`). Each `Dto` is named after its method and ends in `Dto`.

Parameters follow the same idea. The id of the entity a method acts on is `id`, whatever else is passed with it. Here the entity is the module's own entity, or the one named by the Dto, socket payload or client `Params`: `CloseConversationDto { id, userId }`, `IsConversationUser(id, userId)`, sessions' `Delete { id }`, `CloseConversationSocketEventPayload { id }`, `UserOfflineSocketEventPayload { id }`. The name and the class already say which entity it is. There are three exceptions:
- `By<Key>` names the field (`FindMessagesPageByConversationIdSocketEventPayload { conversationId, cursor }`).
- Another entity's id stays `<entity>Id` (`conversationId` in `MessagesService` and `CreateMessageSocketEventPayload`). This includes payloads whose event doesn't name the entity, such as `sessionId` in `AUTHENTICATE`, `LOGGEDIN`, `LOGOUT` and `REGISTERED`.
- An id with another role keeps a descriptive name, even for the entity the method acts on. Examples are `beforeMessageId` in `FindByConversationIdBefore`, and `messageId` in `MarkMessagesRead`, which means "read up to this message".

JSON keys follow the field names.

Client Api classes drop `By` everywhere: methods (`FindAllOpen()`, `FindPage(...)`), `Params` (`FindMessagesPageParams`), events (`MessagesPageFoundEvent`) and subjects. Their `Params` follow the same `id` rule (`CloseConversationParams { id }`).

Services never call their own public methods — shared logic goes into a private helper instead. `SessionsService::IsExpired` and `GenerateExpirationTime` are examples. Calling *another* service's public methods is fine, and returns views.

### Connections

`SocketServer` keeps two indexes that must agree:

- `m_connectionsMap` — socket → `Connection` (which holds an optional user id, the socket's `Framer` and its write buffer)
- `m_userClientSocketsMap` — user id → sockets, for fan-out to someone on several devices

`BindConnectionUser` and `UnbindConnectionUser` are the only things allowed to touch them, and `BindConnectionUser` unbinds first. That makes rebinding safe, which matters because one TCP connection can be bound to several users over its life — the client keeps a single connection for the whole app run, so logging out and back in as someone else reuses the same socket.

`UnbindConnectionUser` is idempotent, so the logout handler, `CloseClientSocket` and `BindConnectionUser` can all call it without checking first.

**Presence hooks.** `OnUserBound(owner, &X::HandleUserBound)` and `OnUserUnbound(owner, &X::HandleUserUnbound)` add a listener, never replace one, so any module can listen (presence now, server metrics later). Each event has its own context struct, `UserBoundContext` / `UserUnboundContext`, holding the user id, so one can gain fields without touching the other.
- **Bound** fires only for a user's **first** socket. A second device fires nothing, and re-authenticating as the same user on the same socket returns early, so there's no offline → online flicker.
- **Unbound** fires only when a user's **last** socket is unbound. The listeners run after both indexes are updated, so a listener sees the user gone from `m_userClientSocketsMap`, and a `SendAll` from it skips the closing socket.
- The hooks fire on bind / unbind, not on TCP connect: a raw connection exists before login and has no user.
- Listeners must not throw: unbind also runs from `CloseClientSocket`, outside any route.

A connection stores only the user's id, not a `UserView`. Every handler only needs the id, and a copy of the user taken at login would go stale on a rename. Code that needs the full user fetches it through `UsersService`. `GetConnectionUserId` returns `nullptr` for a socket that isn't authenticated.

## Client

`client/src/main.cpp` is wiring only — loggers, socket, GUI, stores, APIs, navigation, screens — then the render loop. It performs no actions.

- **Layers** are screens (`SplashLayer`, `LoginLayer`, `RegisterLayer`, `ChatLayer`) with an `OnAttach` / `OnUpdate` / `OnRender` / `OnDetach` lifecycle, held in a `LayerStack`.
- **Navigation** owns the layers for the app's lifetime and pushes them onto the stack.
- **APIs** (`AuthApi`, `ConversationsApi`, `MessagesApi`, `UsersApi`) are transport: they serialize and send, register socket handlers, and notify a `Subject<TEvent>` so layers can subscribe without knowing about sockets.
- **`SocketClient`** allows several handlers per event name. Each handler is registered as an owner and a method, `On(name, this, &UsersApi::HandleUsersSearched)`, and each Api's handlers are private `Handle*` methods. The owner is a pointer because it is also the key: `Off(name, this)` removes only that owner's handler. The private `AddHandler` rejects a null method and logs an Error when the same owner registers the same event twice. `SocketClient` takes a `Logger`. Handlers receive a `SocketClient::EventContext`, the same shape as the server's (only `serializedPayload` today), so new per-event data can be added without changing every handler's signature.
- **Stores** are local persistence. `SessionStore` is the only one so far.
- **`MessageHistories`** (`client/messages/`) holds the loaded messages, next cursor and loading flag of each conversation, keyed by conversation id. It has no GUI or network dependency: `ChatLayer` decides when to fetch and calls `Has` / `Add` / `AppendLiveMessage` / `StartLoading` / `MergePage` / `Remove` / `Find`, so the history rules live in one place.
- **`MessagesReadStates`** (`client/messages/`) holds the read state of each open conversation, keyed by conversation id, with the same shape and no GUI or network dependency. `SetAll` (login) and `Set` (one conversation) store what the server sends. `ApplyLiveMessage(message, isUnread)` applies a `MESSAGE_CREATED` message: it raises `latestMessagePosition` and adds 1 to the count only if `ChatLayer` says the message is unread. It stores no message, which is why it isn't called `AppendLiveMessage` like the `MessageHistories` method. `Find` returns `nullptr` when there's no read state yet. `Remove` and `Clear` are called on close and logout.
- **`MessagesFirstUnreadMatcher`** (`client/messages/`) finds where the "new messages" divider goes: the first message after the read position captured when the conversation was selected, not sent by the user. It's called per message inside `ChatLayer`'s draw loop (`Match`, then `EndFrame`), so the messages aren't walked twice. See "On the client" under Read state.
- **`ScrollAnchor`** (`gui/`) keeps a scrolling list on the same item when older items are prepended. The layer owns one per list and calls it during the draw: `ConsumeCorrectedScrollY` before the container, `MeasureItem` for each item, `EndFrame` at the end, and `Start` before merging an older page. `IsSettled` tells the load trigger when the scroll position can be trusted, and `HasAnchor` tells the auto-scroll to stay out of the way. ChatLayer keeps the loading decisions. `ScrollAnchor` only knows about ImGui frame timing.

**UI sizes derived from another value are always multiplied.** When a layer makes a size or position smaller or bigger than another value, it writes `value * 0.75f`, never `value / 1.33f`. The factor then reads directly as the percentage (75% of the value), and every derived size in a layer reads the same way. Example: the unread badge is `conversationContainerAvailableSpace.y * 0.75f`, not `/ 1.33f`.

Layers follow one shape: call an API method, subscribe to the matching subject, navigate when it fires. Subjects live only on the Api classes and fire on server events. State classes such as `MessageHistories` have none, so a follow-up to a server event (for example marking messages read once the first history page is merged) runs in the layer's handler for that event, right after it updates the state.

**Chat state lives in `ChatLayer`, not in a shared client state object** (decided 2026-11-25). `ChatLayer` owns the conversations list, the selection, `MessageHistories` and `MessagesReadStates`, and `AuthApi` holds the current user. `ChatLayer` is the only code that reads this state. Extracting it now would mean designing a shared object without knowing what a second reader needs, such as observers or read-only access. That will be decided when a second reader actually arrives. The expected one is local OS notifications, which need the sender's name and so need the conversations list outside `ChatLayer`. The rules already live in their own classes, so moving them later is mostly moving members. The cost is accepted: `ChatLayer.cpp` stays large. If a shared object is introduced, it won't be called a "store", because that word means disk persistence in this codebase (`SessionStore`).

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

## Read state (mark-as-read, unread counts)

Status: the server side is built: storage, message positions, the repository counting, `MarkRead`, the read states at login, the read state of one conversation, and their routes. On the client, `MessagesApi` sends the login request, the one-conversation request and `MarkRead`, and `ChatLayer` keeps every read state in `MessagesReadStates` (login, marks, opens, reopens, live messages, close and logout). It also sends the mark-read requests (below). The sidebar badges and the divider are built. See `NEXT_STEPS.txt`.

### The four values

| Value | Where | Meaning |
|---|---|---|
| `Message.id` | every message | A UUID that identifies the message. Used for history cursors, dedup and lookups, and it's what the client sends when marking as read. |
| `Message.position` | every message | The message's send order: 1 for the first message stored on the server, then 2, 3, ... It's global across all conversations and assigned by the repository in `Create` (a counter in memory, a `seq` column in Postgres). It's only compared, never interpreted. |
| `lastReadMessagePosition` | `ConversationUser`, one per user per conversation | The position of the last message this user has read here. `0` means they have read nothing yet. |
| `latestMessagePosition` | read state sent to the client | The position of the newest message in the conversation, whoever sent it. `0` means the conversation has no messages. |

`0` is never a real position, because positions start at 1. That's why it can safely mean "nothing read" and "no messages".

**Unread** = messages in the conversation with a position **greater than** `lastReadMessagePosition`, not sent by the user themselves.

**Why messages have no `readAt`.** Read state is one bookmark per user per conversation (`lastReadMessagePosition` on `ConversationUser`), never a field on each message. Everything at or below the bookmark is read for that user. The unread count, the divider and `MarkRead` all work from it, the same way as Slack's `last_read`. A `readAt` on `Message` doesn't work:
  - **Several readers:** in a group conversation, some participants have read a message and others haven't, so one field can't hold it. It would take a read record per user per message.
  - **Writes:** opening a conversation with 50 unread messages would cost 50 writes instead of one bookmark move.

**Read receipts** ("Seen" under a message, like WhatsApp or iMessage) don't need a per-message field either. A message is seen by Bob if its position is at or below Bob's bookmark. A "Seen at 3:42" time would be one `lastReadAt` next to the bookmark on `ConversationUser`, not a timestamp on every message. Read receipts are a product decision for later. Slack doesn't show them, so the Slack-style feature doesn't need them. Until then, read positions stay off `ConversationUserView`, because that view goes to every participant and sending positions in it would amount to read receipts.

### Worked example

Two conversations share the server. Alice and Bob talk in conversation A. Something else happens in conversation B.

| position | conversation | sender |
|---|---|---|
| 1 | A | Alice |
| 2 | B | ... |
| 3 | A | Bob |
| 4 | A | Bob |
| 5 | B | ... |
| 6 | A | Alice |
| 7 | A | Bob |

Alice's `lastReadMessagePosition` in A is `3`, so she has read up to Bob's message 3.

- Messages in A after 3: 4 (Bob), 6 (Alice), 7 (Bob).
- 6 is Alice's own, so it doesn't count.
- **Alice's unread count in A = 2** (messages 4 and 7). **`latestMessagePosition` = 7**.

### Marking as read: `MarkRead`, step by step

The client sends `MARK_MESSAGES_READ` with the **id** of the newest message it is **displaying**. That isn't necessarily the newest one on the server: a message that arrived but isn't on screen yet must stay unread.

It's sent whenever new messages become visible in the selected conversation:

- **When a conversation is opened for the first time**, once its first history page is merged (`ChatLayer::HandleMessagesPageFound`, right after `MergePage`)
- **When the user switches back to a conversation that's already loaded** (`SelectConversation`)
- **When a new message arrives in the conversation being viewed** (`MESSAGE_CREATED`), unless the user sent it (own messages are never counted)

All three go through two private `ChatLayer` helpers. `IsConversationDisplayed(conversationId)` is true when the conversation is selected and its history is loaded. `MarkMessagesRead(conversationId)` sends the id of the last message in the history, which is the newest one shown, because the history is kept oldest first. It sends nothing when the history is empty. Older pages don't trigger a mark (they go above what's already shown), and neither does a page that arrives after the user switched to another conversation.

Window focus isn't checked yet, so a message drawn while the app is in the background counts as read. Scroll position isn't checked either: opening a conversation scrolls to the bottom, so its newest messages count as seen.

Say Alice's screen shows up to message 6, and she sends message 6's id.

1. **`FindById(id of 6)`** → message 6, with its position `6`. The service then checks that the message belongs to A. If no message has that id, or it's in another conversation, the answer is `UNKNOWN_MESSAGE`. The conversation check matters: without it, a message from another conversation with a higher position would move A's read position past messages Alice never saw.
2. **`AdvanceLastReadMessagePosition(A, Alice, 6)`** → the stored value is `3`, and 6 > 3, so it becomes `6`. It returns what's stored now: `6`. If Alice isn't in A, the answer is `NOT_CONVERSATION_USER`.
3. **`CountByConversationIdAfter(A, afterPosition = 6, excluded = Alice)`** → messages in A after 6: only 7 (Bob). **count = 1, `latestMessagePosition` = 7.**
4. The server sends `MESSAGES_MARKED_READ` with `{ conversationId: A, lastReadMessagePosition: 6, latestMessagePosition: 7, unreadCount: 1 }` to **all** of Alice's sockets, so her other devices update too.

Note that the count is **1, not 0**. Message 7 reached the server but wasn't on Alice's screen, so it's still unread. That's why the client never sets the count to 0 itself and always takes the state the server sends.

**What happens to message 7 next.** It only stays unread for a moment:

1. `MESSAGE_CREATED` for message 7 reaches Alice's client, a moment after the reply above or even before it.
2. The client shows it, because Alice is looking at conversation A. It does **not** add 1 to the badge, because live messages in the selected, loaded conversation are excluded.
3. Showing it is the third trigger (a new message arrives in the conversation being viewed), so the client sends `MARK_MESSAGES_READ` with message 7's id.
4. The server runs the same steps: `FindById` → message 7 (position `7`), `AdvanceLastReadMessagePosition` moves 6 → `7`, and `CountByConversationIdAfter(7)` finds nothing. It replies `{ A, lastReadMessagePosition: 7, latestMessagePosition: 7, unreadCount: 0 }`, and the badge goes to 0.

So the count goes 1 → 0 within moments, and it only ever counts what Alice really hasn't seen. **A message counts as read once it has been displayed, never just because it reached the server.**

**A late or duplicate mark.** If a request to mark message 4 arrives after that (an old request, or another device), step 2 sees 4 is not greater than 6 and changes nothing. It still returns `6`, steps 3 and 4 run the same way, and the answer is `OK`. The request can be repeated safely, and the position never moves back.

### Why the compare happens inside the write

If the service read the position, compared it, then wrote it, two requests running at the same time could do this:

1. Request X (mark 7) reads `3`.
2. Request Y (mark 5) reads `3`.
3. X writes `7`.
4. Y writes `5`. **The position has moved backwards.**

`AdvanceLastReadMessagePosition` does "write only if greater" as **one** storage operation: one `UPDATE ... WHERE last_read_message_position < $new` in Postgres, and one method under one lock in memory. Nothing can run between the check and the write. This is why the position is stored, rather than the message id: an id would need its order looked up in another table first, and that brings back the read, compare, write race.

### How `CountByConversationIdAfter` walks the messages (in memory)

`m_messages` holds every conversation's messages in position order. `CountByConversationIdAfter(A, afterPosition = 3, excluded = Alice)` walks it **backwards, newest first**, so it can stop early:

| message seen | what happens |
|---|---|
| 7 (A, Bob) | first message of A seen, so `latestMessagePosition = 7`. 7 > 3, Bob ≠ Alice → **count 1** |
| 6 (A, Alice) | 6 > 3, but it's Alice's own → not counted |
| 5 (B) | other conversation → skipped |
| 4 (A, Bob) | 4 > 3, Bob → **count 2** |
| 3 (A, Bob) | 3 ≤ 3, already read, and everything older is too → **stop** |

Result: count = 2, `latestMessagePosition` = 7. The `latestMessagePosition == 0` check in the loop means "nothing from this conversation has been seen yet". Because the walk is backwards, the first match is the newest message, and it's stored only once.

Cost: the walk covers only the messages since the read position, so it's tiny for someone who is caught up. `FindById` also walks backwards, because the message being marked is almost always among the newest.

### At login

The client asks `FIND_ALL_OPEN_MESSAGES_READ_STATES_BY_USER_ID` once the conversation list arrives. The server reads every open conversation's stored position (one call) and counts all of them in one batch (one call). That's two round trips, however many conversations there are, and it happens once per login, not per message.

The batch count is `CountByConversationIdsAfter`, the login version of `CountByConversationIdAfter`. `CountByConversationIdAfter` counts one conversation, and calling it in a loop would mean one scan (later one Postgres query) per conversation. The batch method takes every conversation with its read position and returns one `MessagesCount` per conversation from a single pass:

```
in:  { c1 → 3, c2 → 0, c5 → 12 }                     conversationId → lastReadMessagePosition
out: { c1 → {count 2, latest 7}, c2 → {count 1, latest 4}, c5 → {count 0, latest 12} }
```

Each message follows the same rules as in `CountByConversationIdAfter`, applied to its own conversation. A conversation not in the input is skipped. The first message seen for a conversation sets its `latestMessagePosition`. A position at or below that conversation's read position is already read. A sender other than the user counts 1. In memory, the walk stops once a position is **below** the smallest read position, because from there everything older is read in every conversation. It stops below it, not at it: the message at the smallest read position can be the newest message of its conversation (a read position is always a message of that conversation, since `MarkRead` checks it). If c1 has read up to 3 and message 3 is its newest, stopping at 3 would leave c1's `latestMessagePosition` at 0. Visiting 3 sets it to 3, and the walk stops at 2. Unlike `CountByConversationIdAfter`, a read message doesn't end the walk (other conversations may still have unread messages further back), and every input conversation starts at `{count 0, latest 0}`, so a conversation without messages still gets an entry.

`FindAllOpenReadStatesByUserId` chains the two calls: positions from `ConversationsService`, then `CountByConversationIdsAfter`, then one `MessagesReadStateView` per conversation. The server replies with `ALL_OPEN_MESSAGES_READ_STATES_FOUND_BY_USER_ID`, a list of those views in no particular order (the client looks each one up by `conversationId`). The names mirror the conversation list (`FIND_ALL_OPEN_CONVERSATIONS_BY_USER_ID`), because both cover exactly the same set: one entry per open conversation. The JSON converters for `MessagesReadStateView` live in `MessageJson`, since JSON files are grouped by module, like `ConversationJson`, which holds both `ConversationView` and `ConversationUserView`.

The positions don't come from the conversation views, which deliberately leave them out so other participants can't see who read what. `ConversationsService` has a separate storage-only method, `FindAllOpenLastReadMessagePositionsByUserId`, that reads them from the stored records without building any view. It returns a map of conversationId → `lastReadMessagePosition`, which goes straight into the batch count.

That method has its **own repository method** with the same name, rather than reusing the repository's `FindAllOpenByUserId` and picking the positions out in the service. Both are one round trip in Postgres. The difference is what each round trip carries:

| | Reusing `FindAllOpenByUserId` | Dedicated method |
|---|---|---|
| Query | `conversation_users` joined to `conversations`, joined again to `conversation_users` to load every participant | `SELECT conversation_id, last_read_message_position FROM conversation_users WHERE user_id = $1 AND is_open` |
| Rows (20 open conversations × 5 people) | 100, one per participant, every column | 20, one per conversation, two columns |
| Server work | builds 20 full `Conversation` objects, keeps 20 numbers | fills a map of 20 pairs |
| Grows with | participants | conversations |

With an index on `conversation_users (user_id)`, Postgres goes straight to the user's rows, and it can answer from the index alone if the index includes the two columns. In memory, the gain is negligible. The rule behind the choice: a repository method returns what the caller needs, not a bigger object for the caller to trim.

**Compared with Slack** (from its public API shapes, its internals aren't public). Slack also stores `last_read` per user per channel, on the membership. Its older API put the caller's own `last_read` and `unread_count` on each channel object in the list (one call). Its newer web client splits this into `client.boot` (the channel list) and `client.counts` (per channel `last_read`, `latest`, `has_unreads`, `mention_count`), so the costlier counts don't delay the list. We follow the newer split:

| | Slack (newer) | This app |
|---|---|---|
| Conversation list | `client.boot` | `FIND_ALL_OPEN_CONVERSATIONS_BY_USER_ID` |
| Read state / counts | `client.counts` | `FIND_ALL_OPEN_MESSAGES_READ_STATES_BY_USER_ID` |
| Where `last_read` is stored | channel membership | `ConversationUser` |
| Who computes counts | server | server |

Putting the caller's position on `ConversationView` (Slack's older way) was rejected. It would save one repository read per login, but `ConversationView` is also sent to other participants, so it would need a different view per recipient, and one mistake would leak read positions. The cost of the split is that the conversations repository is read twice per login (list, then positions): one cheap extra query per login, never per message.

Cost: in Postgres the count is one `GROUP BY` query on the `(conversation_id, seq)` index. Each conversation jumps straight to its messages after its read position, so only unread rows are read. The in-memory store has no such index, so its batch scan goes back to the oldest read position among the user's conversations. One conversation where nothing has been read means scanning every stored message once per login. That's a limitation of the temporary store, not of the design. A simpler in-memory version without the early stop (reading the whole list every time) was considered and gives the same result; the early stop was kept. The one slow case left in Postgres is a conversation with a huge number of unread messages. If that ever matters, counts can be cached or capped ("99+") inside the repository without changing any interface.

**Two calls, not one query.** Postgres could compute positions and counts in a single round trip: a query on `conversation_users` for the user's open conversations, with two `LATERAL` subqueries on `messages` per conversation (the newest `seq`, and `COUNT(*)` of rows after the read position not sent by the user), both using the `(conversation_id, seq)` index. It isn't used because it crosses module ownership: `conversation_users` belongs to the conversations module and `messages` to the messages module, and each repository only touches its own tables. Keeping two calls costs one extra round trip per login. If login latency ever matters, a dedicated read-state query can replace the two calls below `FindAllOpenReadStatesByUserId` without changing its signature.

### On the client

- **`MessagesApi`** sends `FindAllOpenReadStates()`, `FindReadState({ conversationId })` and `MarkRead({ conversationId, messageId })`, and exposes three subjects: `AllOpenMessagesReadStatesFound` (the list at login), `MessagesReadStateFound` (one conversation, to the asking socket) and `MessagesMarkedRead` (one read state, received by every device of the user).
- **The stored state is set** by every read state the server sends. `MessagesReadStates::SetAll` sets the whole list at login. `Set` adds or overwrites one conversation on `MESSAGES_MARKED_READ` and on the one-conversation reply. Nothing else writes the count, so the server's value always wins, and a mark that was ignored needs no special handling. The methods are named `Set` rather than `Replace` because an entry may not exist yet (the same meaning as `Json::Set`).
- **Conversations that open after login** (opened by the user, or reopened by an incoming message) have no read state yet, because read states arrive once at login. When `CONVERSATION_OPENED` or `CONVERSATION_REOPENED` arrives, the client asks the server for that one conversation's read state with `FIND_MESSAGES_READ_STATE_BY_CONVERSATION_ID`. The server answers `MESSAGES_READ_STATE_FOUND_BY_CONVERSATION_ID` to the asking socket only (`MessagesService::FindReadStateByConversationId`: the user's position from `ConversationsService::FindLastReadMessagePositionById`, which also checks that the user is in the conversation, then one `CountByConversationIdAfter`). Both sides are built: `ChatLayer` sends the request from its `CONVERSATION_OPENED` and `CONVERSATION_REOPENED` handlers. The client never invents one: a read position of 0 would put the divider above messages the user already read. Two alternatives were rejected:
  - **Putting the read state in those events.** A read state is per user, but `CONVERSATION_REOPENED` is serialized once and sent to every reopened user, so it would need a different payload and a count query per recipient inside message creation.
  - **Refetching every read state.** That re-reads all open conversations to update one badge.

  The request is sent from the client's handler for the event, never at the same time as it, so the conversation is visible for one round trip **without a badge**. Login works the same way (the list first, then the read states). The sidebar rule is: no read state for a conversation means no badge and no highlighted name, so nothing is ever shown wrong, only briefly missing. Any `MESSAGE_CREATED` that arrives around the request is counted exactly once whatever the order. Before the reply there's no entry, so the message is ignored and the reply's count includes it. After the reply, the double-count guard applies.
- **A live message (`MESSAGE_CREATED`) adds 1** to the unread count only if `message.position > latestMessagePosition`, and then `latestMessagePosition` is raised. With several threads or servers, a read state that already counted message 7 can arrive **before** the `MESSAGE_CREATED` for 7. Without this check, 7 would be counted twice. Own messages and the selected, loaded conversation don't add 1. `ChatLayer` decides that (`isUnread`) because it knows the current user and the selection; the class only applies the position check. In `ChatLayer::HandleMessageCreated` the message is appended to the history first, then applied to the read state, then marked read if it's on screen. The order doesn't change the result, it follows what happens. The same check decides both: a message from someone else in the conversation on screen isn't counted and is marked read right away.
- **The sidebar badge:** a conversation with `unreadCount > 0` gets a bold, white name and a red rounded badge with the count on the right of its row. No read state, or a count of 0, draws the row as usual: regular weight, grey name, no badge. This follows Slack, where an unread name is both bold and brighter and read names are dimmed grey. Bold alone would leave both the same color, and the color is what makes unread rows stand out. Slack only shows the count badge for mentions on channels, but every DM message counts as a mention, so an unread DM gets the badge too. All conversations here are DMs and mentions are deferred, so every unread conversation gets the badge. The close button already sits on the right of the row but only appears on hover, so the badge shows when the row isn't hovered and the close button when it is, the same swap Slack does between the badge and the row actions. The row ends with early returns: hovered draws the close button and stops, no unread messages stops, otherwise the badge is drawn. `hasUnreadMessages` folds in the `nullptr` check, because the row is drawn every frame and reaches the moment before the read state arrives (a missing check crashed the client on open and reopen). The badge is one red container, a circle (`cornerRounding` at half its size) sized from the row, with the count drawn bold and centered inside it. The count is lifted 1 px after centering: ImGui rounds the font's ascent up to a whole pixel, so text sits up to 1 px low, and Lato leaves slightly more room above digits than below. Read names are white at about 70% opacity rather than a flat grey, so they dim correctly on both the normal and the selected row color.
- **The "new messages" divider:** when a conversation is selected, the client saves its `lastReadMessagePosition` **before** marking as read moves it. The red line is drawn before the first loaded message with a higher position. In the example, Alice opens A with a saved position of 3, so the line goes above message 4. It stays there while A is selected, even after the server moves her position to 6. The rule lives in `MessagesFirstUnreadMatcher` (`client/messages/`), with no GUI or network dependency. `ChatLayer` calls `Reset` then `Capture` when a conversation is selected, and `Capture` again when a read state arrives late. `Capture` ignores `nullptr` and keeps the first value, so a later reply never moves the line. A read state arrives after its conversation was selected in three cases:
  - **The user opened a conversation from search.** `HandleConversationOpened` selects it right away, so its read state always arrives after the selection. `HandleMessagesReadStateFound` returns early if the reply isn't for the selected conversation, then passes the event's read state to `Capture`. Passing its address is safe because `Capture` copies the position and never keeps the pointer.
  - **The user clicked a reopened conversation before its read state arrived.** A reopened conversation is added without taking the focus. Its sidebar row shows before its read state arrives, so the user can click it in between. The same handler covers this case.
  - **The user clicked a row right after login.** Nothing is selected at login, so the `Capture` in `HandleAllOpenMessagesReadStatesFound` is usually ignored (`Find("")` returns `nullptr`). But the sidebar rows show as soon as the conversations arrive, before the read states, so the user can click one in between. That handler keeps `Find(m_selectedConversationId)` because its event is a list. While drawing, `ChatLayer` calls `Match(message, currentUserId)` for each message inside its existing draw loop, the same way it calls `ScrollAnchor`. `Match` returns true only for the first message after the captured position that the user didn't send, and `EndFrame` resets that for the next frame. The captured position is a `std::optional`: "not known yet" draws no line, while `0` means nothing has been read. **How the divider is drawn:**
  - **The row:** an auto-resized, transparent row container drawn just before the message, so the scroll anchor measures that message below the divider.
  - **The text:** red bold `" New"` (the badge's red), right aligned. Its leading space is the gap before the line: `GetTextSize` measures it, and it scales with the font.
  - **The line:** a red `Gui::Divider` whose `width` stops it at the text. It's placed at the top plus half the text height, which centers it on the text.
  - **Why not `AlignCenterY`:** it centers in the container's available height, and an auto-resized container has no height on its first frame. The line would jump every time a conversation with unread messages is opened.
  - **`Gui::Divider::width`:** added for this. If it's left unset, `DrawDivider` keeps the full window width, so the search modal divider looks the same.
  - **Height:** `DrawDivider` only draws a line and takes no space, so the row's height is the text's height.

One edge case is known and not fixed. Suppose there are more unread messages than one page. The divider then sits above the oldest loaded message and moves into the older page when that page arrives. The scroll correction overshoots by one divider height, so the view jumps by that much. `NEXT_STEPS.txt` has the planned fix.

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

**Conversation views embed their participants as `UserView`s, fetched in one batch.** The server resolves user info rather than the client, since a client round trip costs far more than a lookup inside the server. `ConversationsService` collects the distinct participant ids of every conversation it returns, calls `UsersService::FindByIds` once (an `unordered_set` of ids, one repository query), and assembles the views from an id → `UserView` map, so there is no per-conversation lookup (N+1). A participant whose user no longer exists is skipped with a warning. Messages carry only `senderId`. The client resolves it against the selected conversation's participants (`ChatLayer::FindConversationUser`), so the sender's name is not repeated in every message and doesn't go stale on a rename. There is no null check on the result, on purpose: the server only accepts messages from participants. That stops holding once participants can leave a group conversation.

**User search is server-side.** `SEARCH_USERS` sends the typed text; the server matches it case-insensitively against email, first and last name and answers `USERS_SEARCHED` with `UserView`s. An empty search returns nothing rather than every user. The caller is not excluded, since you can message yourself. The client debounces the input (300 ms, `core/utils/Debouncer`, polled in `ChatLayer::OnUpdate` since the render loop has no timers), so one request goes out once typing stops instead of one per keystroke.

**Opening a conversation sends only the other people.** The client sends the user ids it picked; the server adds the initiator from the authenticated connection, never from the payload, then sorts and deduplicates them. Picking yourself therefore gives a one-participant conversation, shown as "<first name> (you)". `Open` is idempotent: the same set of users always returns the same conversation, so the client replaces it if it is already listed instead of adding a duplicate, then selects it. The sidebar only changes when the server answers (`CONVERSATION_OPENED` / `CONVERSATION_REOPENED` / `CONVERSATION_CLOSED`), never optimistically.

**Sending a message reaches every participant and reopens the conversation.** `MessagesService::Create` first checks that the sender is a user of the conversation. An unknown conversation and a non-member give the same `NOT_FOUND`, so a client cannot probe which conversation ids exist. Then it validates the text, stores the message, and reopens the conversation for every user who had closed it (`OpenForAllUsers`), returning the message, the conversation and the ids of the users it reopened for. The handler sends those users `CONVERSATION_REOPENED` first, so their client lists the conversation before the message arrives, then sends `MESSAGE_CREATED` to every participant on all their sockets, the sender included, so their other devices get it too. A user with no socket is offline and is skipped quietly. The client shows a message only when the server sends it back, never optimistically.

**Reopened is its own event, not `CONVERSATION_OPENED`.** `CONVERSATION_OPENED` answers the client's own `OPEN_CONVERSATION`, so the client adds the conversation and selects it. `CONVERSATION_REOPENED` means someone else's message brought it back, so the client adds it without taking the focus. Both carry a `ConversationView`, like `CONVERSATION_OPENED` and `CONVERSATION_CLOSED` already do. Separate names keep the client stateless (no "did I ask for this?" flag), let future listeners such as notifications or unread counts subscribe to reopens only, and let the payloads grow apart. Only users who had the conversation closed get it, so an active chat does not resend the whole conversation with every message.

**Message history is loaded in cursor pages, when a conversation is selected.** Selecting a conversation with no history entry creates the entry and sends `FIND_MESSAGES_PAGE_BY_CONVERSATION_ID` with an empty cursor. The server checks membership, fetches 42 + 1 messages before the cursor (send order, newest first, reversed so each page is oldest first), drops the extra oldest one when present and sets `nextCursor` to the id of the new oldest message, otherwise leaves it empty. An empty `nextCursor` is the only "no more pages" signal, a short page is not. The response carries the `conversationId` next to a generic `PageView` (`core/views/PageView`, `core/json/PageJson`), because several conversations can load at once and the protocol has no request id. The cursor is a plain string on both sides, `""` meaning the first page, so the client sends back exactly the `nextCursor` it received.

**The history entry is created when the fetch is sent, and every page is deduplicated.** A history entry means "loading or loaded". Live `MESSAGE_CREATED` messages are appended only to conversations that have an entry, the others are ignored because their first page will include them. Creating the entry before the page arrives keeps a message that lands while the page is in flight, at the cost of a possible duplicate: if the server stored it before building the page, it is in both. Every merged page drops messages whose id is already in the history, then is inserted in front as is (pages are always older than what is there). Closing a conversation erases its entry, so selecting it again fetches fresh.

**Older pages load near the top, and the view stays on the same message.** When the messages view is within one container height of the top, the client sends the history's `nextCursor`. A per-conversation `isLoading` flag in the history entry blocks sending the same cursor again while a page is in flight, so no debounce is needed, and there is no scroll-direction check, so a short history with no scrollbar keeps loading until it fills the view. Prepending a page would otherwise push the view down, so a `ScrollAnchor` (gui library, owned by ChatLayer) anchors on the message that was first before the merge, measures how far it moved, and corrects the scroll through `Gui::Container::scrollY`, which applies `SetNextWindowScroll` before the container begins. The measurement waits one frame, because ImGui gives a child window no height on the frame it first appears. Message containers use the message id as their ImGui id (not the list index), so a prepended page doesn't take over another message's stored size. Only older pages anchor. The first page goes to the bottom through the existing auto-scroll. The trigger also waits until the layout has settled, meaning the bottom of the list hasn't moved since the previous frame. Until then the scroll position is stale, and a check right after the first page arrived would request a second page the user never scrolled to. `ScrollAnchor` is its own class rather than a `Gui` element because it keeps state across frames, while `Gui` elements are rebuilt every frame. The messages container id includes the conversation id, so each conversation keeps its own scroll position during a session.

**Gui element settings are struct fields, and "not set" is `Gui::UNSET`.** Elements are plain structs rebuilt every frame (`Container`, `Text`, ...), so an optional setting like `Container::scrollY` or `Text::height` is a field that the wrapper applies only when it is not `Gui::UNSET` (`-1.0f`). That way every other element keeps ImGui's default behavior. It is a sentinel rather than `std::optional`, like the rest of the wrapper, and it is not `NULL`, which is a null pointer constant and equal to `0`, a valid scroll position. `-1.0f` must never be a real value for a field that uses it. ImGui's own "leave this scroll axis unchanged" value is a separate private constant in `Gui.cpp` (`IMGUI_SCROLL_UNCHANGED`), because it belongs to ImGui's API and must not change if `UNSET` does. Every `Container` needs a non-empty `id` that is unique among its siblings: it becomes the ImGui child window id, and ImGui asserts on an empty id at the root of a window. Since ImGui 1.92 a font is not tied to one size, and ImGui builds and caches each size the first time it is drawn, so there is no font per size to load.

**Fonts: Lato Regular and Bold, one push per text.** ImGui has no bold style, text is drawn with the current font, so bold means a second font file. The app uses Lato, the font of Slack's chat UI, because it has a Bold weight (the previous font, Audiowide, only came in Regular). It is free under the SIL Open Font License, which is why `assets/Lato-OFL.txt` ships next to `Lato-Regular.ttf` and `Lato-Bold.ttf`. `Gui::Init` loads Regular first, which makes it ImGui's default, then Bold, keeping a pointer to it (`m_boldFont`; the font itself is owned by ImGui's atlas). `Text::weight` (`Gui::FontWeight::REGULAR` or `BOLD`) is the only thing a caller sets. The private `Gui::PushTextFont(text)` makes one `PushFont(font, height)`: the bold font or `nullptr` (keep the current one), and `text.height` or `0.0f` (keep the current size). It always pushes, even when nothing changes (that push is valid and does nothing), so `DrawText`, `DrawTextWrapped` and `GetTextSize` always pop with `PopFont()`. `GetTextSize` uses it too, so bold text is measured with the bold font. The one-argument `PushFont(font)` must not be used: it still compiles as an obsolete compatibility function but resets the size to the font's default, ignoring `text.height`. The pops stay explicit rather than in an RAII guard, matching every other push and pop in `Gui`, and ImGui asserts in debug builds if a stack is left unbalanced at the end of a frame. Pushing a font for every text every frame is normal immediate-mode use and costs a stack entry and a cached lookup. Synthetic bold through ImGui's FreeType loader was rejected because it adds FreeType as a dependency.

**Length prefix framing, not a delimiter.** It is the usual choice for C++ servers and binary protocols, the size is known before reading, and it keeps working if JSON is ever replaced by a binary format, where a delimiter byte could appear inside the payload. `Framer` takes and returns `std::string` as a byte container (binary-safe when built with an explicit size), so a binary serializer would need no `Framer` change. `Framer` throws and the socket class logs and closes, so `Framer` needs no logger.

**Both sides drain the socket on every read.** Client sockets on the server are edge-triggered (`EPOLLET`), so epoll does not notify again for bytes left in the socket: `ReadClientSocket` reads until `EAGAIN`. The client does the same in `Read()`, called by its once-per-render-frame `Update()`, otherwise a large payload would take many render frames to arrive. All complete frames are extracted first, then dispatched in order.

**Both sides keep a write buffer, neither waits on `send()`.** A non-blocking `send()` can take only part of a frame, and dropping the rest would corrupt the framed stream. Each `Connection` keeps the unsent bytes in `writeBuffer` and `SendWriteBuffer` sends what it can. On the server, client sockets are registered for `EPOLLOUT` too, and epoll reports when a full socket has room again. On the client, `Update()` retries once per render frame. The client does not block until the socket has room, since that would freeze the render loop. On macOS `send()` during a non-blocking connect returns `ENOTCONN`, so the client keeps the bytes and retries, the same as for `EAGAIN`. The server never closes inside `SendTo`, because `SendAll` loops over `m_connectionsMap`: on a send error it drops the buffer and lets epoll's `EPOLLERR` / `EPOLLHUP` close the socket. SIGPIPE is disabled (`MSG_NOSIGNAL` on Linux, `SO_NOSIGPIPE` on macOS) so a peer that is gone returns an error instead of killing the process.

**Logic lives on the server, the client stays thin.** Rules and decisions such as validation, membership, paging, which conversations reopen, and what counts as unread are made on the server. The client keeps only its own UI state (loaded history, selection, scroll) and shows what the server sends. When it has to compare anything, it compares opaque values the server gave it and never applies a business rule.

**The server is designed for several instances and threads, even before it has them.** Every check followed by a write is one repository method: one conditional statement in Postgres, and one critical section in memory once the repositories get a mutex. It is never two service calls, because another request could run in between. A repository method locks only its own data and never calls another repository while holding the lock, so locks are never nested. One stored value is preferred over two that must stay consistent. Server calls are kept few, because each one becomes a Postgres round trip: batch lookups instead of one per item, services don't build data they then throw away, and repository methods return what the caller needs rather than a bigger object for the caller to trim.

**Read position is a message position, not a message id.** (Full walkthrough with an example in "Read state" above.) Each message keeps its UUID as its identity and gets a `position`: a send-order number assigned by the repository (a counter in memory, a `seq` identity column in Postgres). It is opaque and only ever compared. Each user stores `lastReadMessagePosition` per conversation, `0` meaning nothing read. Marking as read is a compare-and-set (`AdvanceLastReadMessagePosition`) that only moves forward, so concurrent marks can never move it back, and a late or duplicate mark is a no-op that still answers `OK`. Unread counts are computed (messages after the position from other senders), never stored, and the server always sends back the full read state rather than letting the client reset its count. Read states carry `latestMessagePosition` and messages carry their `position`, so a live message that is already included in a read state is not counted twice. Storing an id would need its order looked up elsewhere, which turns the update into read, compare, then write, and that is a race once there are several threads or instances. Storing both an id and a position would create a pair that could disagree. This follows Slack's model, where `last_read` is a timestamp and therefore ordered.

**Stores are per build type.** A Debug and a Release build are different binaries with different baked-in constants, so they can't collide on one session file.

**JSON for the session file even though it holds one field.** Consistency with the settings store that will hold several.

## Planned: telemetry dashboard

Once the chat is done, the repo gets a fourth app: `dashboard/`, a standalone insights and telemetry dashboard for the chat server, built with Unreal Engine's Slate UI framework. It's planned, not started. The full plan is in `NEXT_STEPS.txt`.

**What it is:**
  - **A standalone Slate program,** like the engine's own tool programs, not an editor plugin. It runs without the Unreal editor.
  - **Its own client of the server.** It has its own `MetricsApi` and uses `core` for framing, JSON, socket events, `SocketClient` and the observer. It doesn't use `gui/`, which is ImGui (Slate replaces it), or `client/`.
  - **Fed by a server `metrics` module** in the usual handler, service and repository pattern. The first metrics are connections and sessions, messages per second, latency per route and error counts by code, later per instance.
  - **Aggregate data only.** Counts, rates and latencies. No message text, names or user ids ever reach the dashboard.

**How it builds:**
  - **Unreal's build tool, driven from CMake.** Slate programs are built by UnrealBuildTool, not CMake, and it expects them inside the engine's `Engine/Source/Programs/`. A symlink makes `dashboard/` appear there.
  - **An optional engine path.** `cmake .. -DUE_ROOT=/path/to/UnrealEngine` adds a `Dashboard` target. It recreates the symlink, builds `Core` first, then runs the engine's `Build.sh ChatDashboard`. The paths to `core` reach the dashboard's `Build.cs` as environment variables.
  - **Not part of the normal build.** It's not in `ALL`, so `make` doesn't wait on Unreal. Run `make Dashboard`. Without `UE_ROOT` the target doesn't exist and the rest of the repo builds without Unreal.
  - **The engine version is pinned by a check, not a submodule.** CMake reads the engine's `Build.version` and stops with an error if it isn't the supported version. The UnrealEngine repository is private, so a submodule would fail to clone for anyone without Epic access.
  - **An engine built from source.** The Epic Games Launcher's prebuilt engine can't build standalone programs. Only the `ChatDashboard` target is built, never the editor, which keeps the build much smaller than a full engine build.
  - **`core` in Unreal.** It's linked as a third-party module wrapping `libCore.a`. The module turns exceptions on (`bEnableExceptions = true`), because `core` throws and Unreal modules have exceptions off by default.

**Splitting `core` first:**
  - **One library per subfolder.** `core` stays a toolset, graphics included, and a GLFW `Window` class is coming. So it gets subfolders that each build their own library: `base` (`Core`, plain C++ plus nlohmann_json) and `graphics` (`CoreGraphics`: `Texture`, `Window`).
  - **The rule.** A file leaves `base` only if it needs a library `base` doesn't have. That's why `Layer`, `LayerStack` and the observer stay in `base`.
  - **Who links what.** The dashboard links `Core` only.
  - **Not decided yet.** Whether `SocketServer`, which needs epoll-shim on macOS, gets its own `server` library.

## Known gaps

- `SocketClient::Connect` and `SocketServer`'s setup call `exit()` instead of reporting the failure.
- The write buffers have no cap: a peer that never reads makes them grow without limit.
- Group conversations exist on the server (up to 9 users) but the client can only start one-on-one conversations, one search result at a time. If participants can ever leave, the client's sender lookup needs a null check (a former participant's old messages would point to a missing user).
- Expired and orphaned sessions are never deleted from the repository.
- The in-memory repositories have no locks (plain vectors). Each one needs a mutex before the server goes multithreaded.
- Presence is login-based and wrong with several devices: logging out on one sends `USER_OFFLINE` while another is still connected, and closing the app sends nothing. Planned: presence based on authenticated connections (online when a user's first socket is bound to them at login, offline when their last one is unbound), not on raw TCP connections, which exist before login. `SocketServer` now has the `OnUserBound` / `OnUserUnbound` hooks (see Connections), with nothing listening yet. A `UsersPresenceNotifier` in the users module will send the presence events. It's not a handler, because no client request triggers it. It's given `SocketServer` to send, and `UsersModule` binds its `HandleUserBound` / `HandleUserUnbound` methods next to its route, the same way handlers are wired.

See `NEXT_STEPS.txt`.
