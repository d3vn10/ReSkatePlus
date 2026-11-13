# ReSkatePlus multi-symbol dependency contracts

Build namespace: `dingosdk::game::build::v20260929`  
Repository: `d3vn10/ReSkatePlus`, branch `main`  
Parent inventory: [dependency contract map](reskateplus-dependency-contract-map.md)

This expansion breaks eight dense headers into their independent contracts. It intentionally separates **what the symbol does** from **whether its ABI is fully recovered**. The source tree keeps build-specific addresses in these headers and feature code consumes them through the address namespace; the table below is therefore organized around ReSkate's actual integration boundary, not around a speculative replacement implementation.

## Status vocabulary

- **Contract described**: header comments or nearby declarations specify the native role and important data semantics.
- **ABI needs call-site confirmation**: symbol is identified, but exact parameter/return types and ownership must be checked in its consumer and native caller.
- **Data identity**: vtable, TypeInfo, global, offset, or constant. Match the referenced object/layout, not a function body.
- **Patch group**: multiple code sites or bytes must be treated as one coherent behavior.
- **Shared helper**: used by more than one feature; changing it can affect unrelated subsystems.

No declaration below is evidence that a replacement exists in the newer executable.

---

## 1. `engine.h` — shared engine primitives

This header is a registry of shared services, object identities and context globals. It is not one monolithic dependency. The same primitive can support several higher-level contracts.

### Callable functions

| Symbol | ReSkate-side contract | Inputs / outputs / required behavior | Priority / status |
|---|---|---|---|
| `ui_control_destroy` | Shared destructor used by cosmetic/music list control vtables. | Receives a UI control object as `this`; follows its native control/vtable destruction path. Must preserve ownership of nested controls. | P2, ABI needs call-site confirmation |
| `audio_manager_update` | Audio manager vtable slot +0x20. | Called as a virtual update on the native audio manager; frame/update context is determined by vtable ABI. | P2, data identity |
| `check_entitlements` | Native entitlement request entry. | Starts the native entitlement request flow. The meaningful result is asynchronous; request state, continuation, promise/future and error ownership are part of the behavior. See `entitlement_trace.h` and `fixed_stop_entitlement.h`. | P2, async contract |
| `construct_error` | Constructs native engine error value. | Destination error object plus native code/message/context inputs; output must be a correctly initialized native error with destructor-compatible ownership. Also used by party request result construction. | P2, ABI needs call-site confirmation |
| `animation_count_getter` | Animation definition count getter. | Reads definition +0x1a0, then count at +0x0c. Return is the count; object layout is the real dependency. | P2, contract described |
| `native_text_release` | Releases engine-owned text/string handle. | Consumes a native string/text object and releases its owned storage/reference. Must match native string representation and avoid double release. | P1/P2, shared helper |
| `string_assign` | Native string assignment. | Assigns source text into a native string object; callers may pass source chars and length or another native string depending on call site. Must preserve allocator/ownership and empty-string behavior. | P1/P2, shared helper |
| `current_context` | Gets current thread's game context. | No explicit args; returns context for the calling thread. Context is thread/realm sensitive; it cannot be cached and reused across unrelated worker or realm contexts without validation. | P0/P1, shared helper |
| `settings_lookup` | Looks up settings descriptor in native settings manager. | Manager/descriptor input, returns matching native setting object or null. The descriptor and TypeInfo are part of the lookup contract. | P1, shared helper |
| `model_find` | UI data model lookup. | Model manager/handle or model identity inputs; returns native model reference/record. Must be paired with model lifetime and lock rules. | P2, shared helper |
| `query_context_constructor` | Constructs native game query context. | Initializes caller-provided query context with native defaults/realm state. Query callers must preserve its full expected size/alignment and teardown rules. | P1, shared helper |
| `board_render_pose` | Publishes skateboard render pose. | Supplies board/render object and pose payload to renderer; pose layout, frame ownership and render-thread phase matter. | P1/P2, ABI needs call-site confirmation |
| `assign_future` | Assigns a future from promise state. | Future destination and promise/shared-state source; updates shared-state reference count and destination pointer. A shallow pointer copy is not equivalent. | P2, contract described |
| `array_allocate` | Native engine array allocation. | Wrapper/header plus allocation size/alignment; returned block follows engine array header convention. Allocation must use the allocator compatible with the owning native object. | P1/P2, shared helper |
| `delegate_copy` | Copies native delegate. | Destination/source delegate; preserves callable target and captured state/refcount semantics. | P1/P2, shared helper |
| `delegate_destroy` | Destroys native delegate. | Delegate pointer; releases captured callable/target state according to native flags. | P1/P2, shared helper |
| `set_customization_flag` | Sets customization/indicator property flag. | Property object and flag input; writes native state consumed by customization/indicator UI. | P2, ABI needs call-site confirmation |
| `post_event_delayed_frames` | Graph native delayed-event function. | Graph event/delegate and frame delay inputs; schedules event by frame count. Graph native table binds hash 0x9967fc07 to this entry. | P2, graph ABI |
| `post_event_delayed_seconds` | Graph native delayed-event function. | Graph event/delegate and seconds delay; schedules event by time. Graph native table binds hash 0x5e3aee8e to this entry. | P2, graph ABI |
| `complete_promise` | Completes native promise with result/error. | Promise state plus result or error payload; wakes continuations and changes shared-state completion. Callback order and single-completion semantics matter. | P2, async contract |
| `delegate_invoke` | Invokes native delegate. | Delegate and native argument payload; invokes stored target using native calling convention. Delegate ownership/destruction remains separate. | P1/P2, shared helper |
| `card_info_construct` | Constructs native player-card info object. | Destination player-card info; initializes native fields and ownership for local player card integration. | P2, ABI needs call-site confirmation |

### Vtables and object identity

These are **data identities**, not function entry points. A replacement must identify the equivalent class/vtable and preserve the slots ReSkate uses.

| Symbol | Object identity / use | Priority |
|---|---|---|
| `controller_vtable` | Connection controller. | P2 |
| `allocator_adapter_vtable` | Engine allocator adapter. | P2 |
| `source_manager_vtable` | Client skater/source manager. | P1 |
| `skater_audio_component_vtable` | Skater audio component. | P2 |
| `audio_manager_vtable` | Audio manager; includes update slot +0x20. | P2 |
| `client_vtable` | Client game context; state field at +0xc4 is consumed by transition flow. | P0 |
| `client_skater_source_vtable` | Client skater source. | P1 |
| `skater_entity_vtable` | Skater entity. | P1 |
| `cosmetics_manager_vtable` | Cosmetics manager. | P2 |
| `server_vtable` | Current game server. | P0/P1 |
| `game_settings_vtable` | DelMarGameSettings. | P1 |
| `skater_component_vtable` | Skater component. | P1 |
| `board_component_vtable` | Board component. | P1 |
| `board_entity_vtable` | Board entity. | P1 |
| `board_data_vtable` | Board data object. | P1 |
| `blueprint_vtable` | Blueprint objects, including board and UI widget blueprints. | P1 |
| `skater_appearance_vtable` | Skater appearance entity. | P1 |
| `camera_vtable` | Camera object. | P1/P2 |
| `board_holder_vtable` | Board holder. | P1 |
| `local_player_vtable` | Local player. | P1 |
| `local_player_manager_vtable` | Local player manager. | P1 |
| `server_player_vtable` | Server player. | P1 |
| `server_player_manager_vtable` | Server player manager. | P1 |

### TypeInfo identities

TypeInfo pointers must be matched by semantic type and reflected layout. A numerically similar type pointer is not sufficient.

| Symbol | Type identity | Priority |
|---|---|---|
| `cosmetic_slot_type` | Cosmetic slot. | P2 |
| `persona_id_type` | Persona ID. | P2 |
| `player_info_type` | Player info. | P1/P2 |
| `board_blueprint_type` | Board blueprint. | P1 |
| `reference_type` | Generic native reference. | P1 |
| `bool_type` | Boolean primitive. | P1 |
| `uint32_type` | Unsigned 32-bit primitive. | P1 |
| `int32_type` | Signed 32-bit primitive. | P1 |
| `uint64_type` | Unsigned 64-bit primitive. | P1 |
| `int64_type` | Signed 64-bit primitive. | P1 |
| `float_type` | Float primitive. | P1 |
| `string_type` | CString native type. | P1 |

### Globals and offsets

| Symbol | ReSkate reads/writes | Priority / notes |
|---|---|---|
| `context_partition_offset` | u32 partition field in a game context. | P1, context layout |
| `context_type_offset` | u32 realm/type field in a game context. | P0/P1, context layout |
| `context_client_connection_offset` | Client connection field in context. | P1 |
| `context_player_manager_offset` | Player manager field in context. | P1 |
| `backend_services` | Pointer to backend services. | P2; may be absent/offline |
| `audio_manager` | Audio manager pointer. | P2 |
| `appearance_manager` | Appearance manager pointer. | P2 |
| `source_managers` | Pointer array indexed by partition * 7 + kind. | P1; array indexing contract |
| `loadout_manager` | Loadout manager pointer. | P2 |
| `cosmetics_manager` | Cosmetics manager pointer. | P2 |
| `client_game_manager` | Client game manager pointer; also party client alias. | P0/P1 |
| `location_owner` | Location owner pointer. | P2 |
| `ui_manager` | UI manager pointer. | P2 |
| `ui_model_registry` | Pointer offset +0x140 in UI manager to model registry. | P2; offset, not RVA |
| `game_server` | Game server pointer. | P0/P1 |
| `game_settings_handle` | DelMarGameSettings handle. | P1 |
| `default_arena` | Default allocator arena pointer. | P1 |
| `empty_cstring` | Engine empty CString object. | P1; string sentinel identity |
| `settings_manager` | Settings manager pointer. | P1 |
| `ui_allocator` | UI allocator pointer. | P2 |
| `entity_creation_ready` | u8 readiness flag for entity creation. | P0/P1; lifecycle gate |
| `player_parent_offset` | u32 registration offset between player base classes. | P1; object-layout relation |
| `domain_owners` | Pointer array indexed by domain. | P1; domain indexing/lifetime |
| `tls_index` | Engine TLS slot index. | P1; thread-local access |

**Key dependency edges:** `current_context` + context offsets + `tls_index` establish realm/thread identity; `client_vtable` + `client_game_manager` + `entity_creation_ready` underpin runtime and entity creation; `array_allocate`, delegates, strings, promises and TypeInfos are shared infrastructure used by party/profile/throwdown adapters. Recovering one of these helpers can validate several dependent contracts, but only if the helper's ABI and allocator domain match.

---

## 2. `runtime.h` — tick and local level request

| Symbol | Contract | ReSkate sends / receives | Priority / status |
|---|---|---|---|
| `client_tick` | Client update hook. | Hook ABI: `void(client, update)`; ReSkate performs per-frame runtime work and forwards the original call with both native arguments unchanged. Tick phase and thread are critical. | P0, caller traced |
| `client_tick_prefix` | Entry validation bytes for `client_tick`. | Guard only; not a separate callable dependency. Prefix bytes must be instruction-aligned and relocation-tolerant when matching a changed build. | P0, patch evidence |
| `menu_expression` | Main-menu expression hook for session failures. | Receives native expression context/arguments; ReSkate observes or handles failure path then preserves original evaluation where required. Exact ABI and branch behavior require consumer trace. | P2, ABI needs call-site confirmation |
| `menu_expression_prefix` | Entry validation bytes. | Guard for menu-expression hook. | P2 |
| `level_description_init` | Constructs native LevelDescription. | Destination description plus native construction inputs; must initialize variant-backed fields and owned members before the local request. | P0, image-check + caller trace |
| `level_description_destroy` | Destroys native LevelDescription. | Description pointer; releases its owned variant/string/reference fields. Pair with constructor and native ownership. | P0 |
| `hosted_level_selector` | Selects whether a level asset loads through local hosted server. | Returns a boolean/selection decision consumed by level loading. Must preserve normal local-vs-hosted behavior. | P1 |
| `local_level_request` | Client vtable +0x70 local level request. | ABI observed in runtime caller: `void(client, description, extra, flags)`; ReSkate sends client, constructed description, null extra, zero flags. | P0, caller traced |
| `ImageCheck` / `image_checks` | Startup identity checks. | Checks bytes at LevelDescription init/destroy, string assign, hosted selector and local request before runtime controller starts. It is a set of independent validation sites, not one function. | P0, patch/identity group |
| `sublevel_list` | Pointer to loaded-sublevel list object. | Read as native list; listener subscription and list lifetime participate in sublevel lifecycle. | P0, data identity |
| `sublevel_list_vtable` | Loaded-sublevel list vtable. | Confirms list class/virtual dispatch. | P0, data identity |
| `sublevel_listener_vtable` | Listener vtable at list +0x40. | ReSkate's listener must be laid out for native callbacks and remain alive while registered. | P0, data identity |
| `level_description_layout` | Type record containing variant offsets +0x20, +0x50, +0x68, +0x80. | Native description copy uses these u16 offsets to locate variant members. Matching the constructor without this layout is unsafe. | P0, layout |
| `context_message_bus_offset` | Message bus offset in game context. | Reads context + offset to access native message bus for level/sublevel notifications. | P0, layout |

**Runtime level request structure:** the ReSkate-side description is 0x30 bytes. Known fields: +0x08 starting point, +0x10 root level asset, +0x18 sublevel starting point, +0x20 sublevel asset, +0x2c request flag. Keep this structure and native LevelDescription's own copy/layout distinct until their relationship is confirmed.

---

## 3. `level_loading.h` — state transition and loading boundary

| Symbol | Contract | ReSkate sends / receives | Priority / status |
|---|---|---|---|
| `level_transition_contract` | Native transition callback entry. | ABI: `void(client, next_state:u32, previous_state:u32)`. Client tick stores next state at client +0xc4 before invoking it. ReSkate runs pre-transition callbacks, then forwards all arguments to original. | P0, caller traced |
| `level_transition_caller_contract` | Caller-side state-store and invocation sequence. | Confirms next/previous state register roles, client pointer, +0xc4 store, and call into transition. This is a separate semantic anchor, not another implementation of the transition. | P0, paired evidence |
| `level_request_contract` | Native level request/state path. | Observes request state/description and native request dispatch. The runtime local request ABI is separately recorded in `runtime.h`; do not merge the two fingerprints without checking their call graph. | P1, observational/ABI needs trace |
| `connection_disconnect_contract` | Native connection close independent of ReSkate's Steam session. | Observes or intercepts native disconnect state/reason. Does not itself represent the ReSkate session disconnect. | P2, observational |
| `loading_screen_select_contract` | `RimeLoadScreen::selectInfo(this, boot)`. | Returns an authored `RimeLoadScreenInfo`; called before next description is delivered during local root reload. ReSkate supplies/selects info, game consumes returned native structure. | P2, ABI described |

**Transition dependency:** pre-transition callback ordering is part of the contract. Callbacks coordinate effects, skater events, profile, challenge attempts, world entities, placement physics, score line, native menu, and Throwdown lab cleanup. A replacement that calls the original transition but changes when callbacks run is not behaviorally equivalent.

---

## 4. `native_party.h` — party/social UI, map POIs and async requests

### Model access helpers

| Symbol | Contract | Inputs / outputs | Priority |
|---|---|---|---|
| `model_record` | Resolves model handle to record in model manager. | Model manager + model handle; returns record or null. | P2 |
| `model_type` | Reads TypeInfo from model record. | Record pointer; returns model TypeInfo. | P2 |
| `model_singleton` | Returns/creates singleton model for registered type. | Model manager/type; returns native model reference. | P2 |

These are shared UI helpers also used by indicator/menu adapters. Their lock and reference semantics must be preserved.

### Player, party and spectate/teleport queries

| Symbol | Contract | ReSkate-side use / expected output | Priority / status |
|---|---|---|---|
| `get_info` | UIPlayerManager lookup by EA ID. | Input EA ID; returns UIPlayerInfo/model reference for a player. | P1/P2, ABI needs call-site confirmation |
| `is_member` | Native party membership test. | Player identity input; returns membership boolean. | P1 |
| `member_ids` | Native party member-ID list. | Returns/outputs native u64 ID array. Array header/capacity/count convention matters. | P1 |
| `members_with_leader` | Party member list with leader flag. | Produces native array of member records including leader indicator; ReSkate hook mirrors native array allocator/header and POD copy. | P1, layout+call |
| `weak_player` | Resolves weak player reference. | Weak ref input; returns live player or null. Generation/lifetime check is essential. | P1 |
| `weak_entity` | Resolves weak entity reference. | Weak ref input; returns live entity or null. | P1 |
| `world_transform` | Reads player/entity world transform. | Entity/player input; returns or fills transform consumed by spectate/teleport. | P1 |
| `player_id` | Resolves native player ID. | Player reference input; returns player identity. | P1 |
| `can_spectate` | Native spectate eligibility predicate. | Player/target state; returns bool. Must account for target liveness/state. | P2 |
| `can_teleport` | Native teleport eligibility predicate. | Player/target state; returns bool. | P2 |
| `spectate` | Starts spectating a target. | Player/target identity; changes native spectate target/state. | P2 |
| `spectate_active` | Reads active spectate state. | Native state input; returns bool/state. | P2 |
| `spectate_clear` | Clears spectate target/state. | Player or controller object; mutates native state. | P2 |
| `spectate_name` | Gets spectate target name. | Native name/string output; string ownership matters. | P2 |
| `teleport` | Native teleport command. | Player/entity and target transform/context; changes world placement. | P2 |

Exact native argument types for these query/command functions need confirmation at their ReSkate call sites before they can become scanner ABI constraints.

### Native array helpers

| Symbol | Contract | Required semantics | Priority |
|---|---|---|---|
| `array_append` | Appends a slot to a native array. | Must update capacity/count and return the appended slot using the native array header. | P1 |
| `id_construct` | Initializes appended 64-bit ID slot. | Constructs u64 ID value in native array storage. | P1 |
| `array_allocate` | Shared engine array allocator. | See `engine::array_allocate`; allocator/header layout must match. | P1 |
| `empty_array` | Shared empty-array sentinel. | Returns native empty-array sentinel. Callers inspect header at array - 4; it is not interchangeable with a null pointer. | P1 |
| `array_free` | Frees native array block. | Must use the native allocator and expected wrapper/data pointer convention. | P1 |

### Map party markers

| Symbol | Contract | ReSkate-side use | Priority |
|---|---|---|---|
| `poi_construct` | Constructs native map point-of-interest value. | Destination POI plus map data; initializes native type/fields. | P2 |
| `poi_destroy` | Destroys native map POI. | Releases POI-owned fields/references. | P2 |
| `map_register` | Registers POI with map manager. | Manager + POI; makes marker visible to map. | P2 |
| `map_unregister` | Removes POI from map manager. | Manager + POI identity; unregister before destroying POI. | P2 |
| `roster_changed` | Dispatches native roster observers. | Notifies native UI/model observers after roster mutation. | P2 |

### TypeInfo and global identities

| Symbol | Contract | Priority |
|---|---|---|
| `player_list_type` | ModelRef array type for social player lists. | P2 |
| `uint64_array_type` | Native u64 array type. | P1 |
| `uint8_type` | Native u8 type. | P2 |
| `vec2_type` | Native 2D vector type. | P2 |
| `matrix_type` | Native matrix type. | P2 |
| `sorted_friends_type` | UIFriendsService sorted-friends-list type. | P2 |
| `map_poi_type` | Map POI type. | P2 |
| `map_data_type` | Map data model type. | P2 |
| `map_poi_kind_type` | Map POI kind enum TypeInfo. | P2 |
| `friends_service` | Pointer to UIFriendsService. | P2 |
| `friends_service_vtable` | UIFriendsService vtable. | P2 |
| `map_manager` | Pointer to map POI manager. | P2 |
| `map_manager_vtable` | Map POI manager vtable. | P2 |
| `party_client` | Alias to engine client game manager; native EA party client. | P1/P2 |

### Async party request API

All requests receive native async handle/delegate objects. A correct substitute must produce the expected ReSkate session command, native result/error, callback invocation and handle cleanup. A boolean return or no-op is not equivalent.

| Symbol | Native request contract | Required result behavior | Priority |
|---|---|---|---|
| `request_invite` | `(Handle*, const u64* eaid, Delegate*)` | Route invite to ReSkate session; construct 0x28-byte result; invoke delegate; clear handle. | P1 |
| `request_join_group` | `(Handle*, u64 group, i32 reason, Delegate*)`; reason 1=direct, 2=invite. | Route group join and complete delegate/handle. | P1 |
| `request_join_player` | `(Handle*, const u64* eaid, Delegate*)`. | Route player join and complete delegate/handle. | P1 |
| `request_decline` | `(Handle*, const String* invite_id, Delegate*)`. | Decline matching invite, return native result, invoke delegate and clear handle. | P1/P2 |
| `request_leave` | `(Handle*, Delegate*)`. | Leave current ReSkate session/party and complete native request. | P1 |
| `request_kick` | `(Handle*, const u64* eaid, Delegate*)`. | Request removal of target; permission/error result must be represented natively. | P1 |
| `request_promote` | `(Handle*, const u64* eaid, Delegate*)`. | Promote target/leader change; callback and result required. | P1 |
| `request_privacy` | `(u32 setting, u32 value)`; setting 4=invite, 5=join. | Changes corresponding native privacy setting. | P2 |
| `result_make` | `(Result*, code, text)`; result is 0x28 bytes, message String at +0, code u32 at +0x20; 0=success. | Constructs native result payload with native string ownership. | P1 |
| `delegate_invoke` | `(Delegate*, Result*)`. | Invokes callback with result. | P1 |
| `null_handle` | `(Handle*, 0)`. | Sets async handle to empty after completion. | P1 |
| `local_is_leader` | `(bool* out)`. | Writes current local leader state. | P1 |
| `in_group` | `() -> bool`. | Reports whether native party/group is active. | P1 |
| `player_group_status` | `(const u64* eaid, GameGroupStatus* out, bool* found)`. | Writes status and found flag for player. | P1 |
| `request_accept_invite` | `(Handle*, const String* invite_id, const String* game_id, Delegate*)`. | Completes with `delegate_invoke_text(Delegate*, DingoError*, const String* game_id)`; must preserve string and error lifetime. | P1 |
| `delegate_invoke_text` | `(Delegate*, DingoError*, const String* game_id)`. | Invokes accept-invite callback with error and game ID. | P1 |
| `voice_unavailable` | `() -> bool`. | Returns true when native party voice options should be hidden without native group. | P2 |

### Native UI event dispatch

| Symbol | Contract | Required behavior |
|---|---|---|
| `current_context` | Shared `engine::current_context`. | Obtain current realm/thread context before dispatch. |
| `event_dispatcher` | Resolves dispatcher from context/event buffer. | Dispatcher pointer is returned; check dispatcher +0x28 live byte before posting. |
| `event_post` | Posts native event. | Receives dispatcher, event TypeInfo, payload, options {f32 0,u32 1,u32 0}, and trailing null argument. |
| `dispatcher_live` | Byte offset +0x28. | Gate dispatch on this flag. |
| `group_join_event` | GroupJoinNotification TypeInfo; payload u64 EAId. | Event payload must be native-layout compatible. |
| `group_leave_event` | GroupLeaveNotification TypeInfo; payload u64 EAId + i32 reason (0 left, 1 kicked, 2 location access). | Preserve reason semantics. |
| `leader_changed_event` | GroupLeaderChangedNotification TypeInfo; payload u64 new leader. | Native observers consume new leader identity. |
| `game_invite_event` | GameInviteUpdated TypeInfo; payload String GameId, String invite ID, u64 inviter, i32 1, bool matchmaking. | Strings must remain valid through dispatch/copy. |

**Fingerprint collections:** `Request`, `party_request_contracts`, and `native_party_contracts` are evidence bundles for groups of request/event functions. The scanner should expand their contained RVAs into individual rows but retain a shared group ID so one confirmed helper can constrain its related family.

---

## 5. `profile.h` — profile, settings, progression, cosmetics and local services

This header is the largest of the eight. It is not one profile function: it contains several independent native integration families. The function names below are kept separate so a future matcher can resolve them independently. Unless an ABI is explicitly described, exact parameter registers and ownership remain **call-site confirmation required**.

### Asset and challenge index

| Symbol | Contract | ReSkate input/output and side effects | Priority |
|---|---|---|---|
| `asset_find_contract` | Named-asset lookup used by native TextureAsset renderer. | Resolves a u16/name-keyed asset from the asset registry; native renderer consumes returned asset pointer. | P2 |
| `challenge_counts_contract` | Reads challenge counts. | Challenge/profile owner input; returns or fills count data consumed by progression/profile UI. | P2 |
| `challenge_index_insert_contract` | Inserts challenge into native challenge index. | Index plus challenge identity/record; mutates index storage and ordering. | P2 |
| `challenge_index_append_contract` | Appends to challenge index array. | Native array/index owner; grows/copies storage as needed. | P2 |

### Profile lifecycle and state

| Symbol | Contract | ReSkate input/output and side effects | Priority |
|---|---|---|---|
| `load_contract` | Native profile load. | Profile object/context; consumes saved/profile data and initializes native profile state. | P1 |
| `ready_contract` | Profile readiness predicate/callback. | Reads profile manager/state; readiness gates consumers. | P1 |
| `send_contract` | Native profile send/update. | Profile plus request/payload; sends native profile update through backend path. | P1 |
| `cache_contract` | Profile cache state. | Reads cache state/flags and updates profile-side cache behavior. | P2 |
| `complete_load_contract` | Completes profile load. | Consumes load result/state and advances native load lifecycle. | P1 |
| `success_contract` | Profile success completion. | Marks request/result successful and updates native status fields. | P1 |
| `destroy_string_contract` | Engine native text release alias. | Destroys native string field; ownership must match engine string helper. | P1 |
| `starter_profile_contract` | Starter profile initialization. | Initializes default profile values/records for a new local profile. | P1 |

### Boolean profile settings

| Symbol | Contract | Inputs / outputs | Priority |
|---|---|---|---|
| `get_bool_contract` | Native profile bool getter. | Profile/setting key, returns bool or writes output. | P1 |
| `set_bool_contract` | Native profile bool setter. | Profile/setting key/value; mutates profile and may trigger dirty/notification behavior. | P1 |

### Onboarding and expression graph integration

| Symbol | Contract | ReSkate input/output and side effects | Priority |
|---|---|---|---|
| `refresh_graph_contract` | Refreshes profile/onboarding expression graph. | Graph/profile context; rebuilds or refreshes graph state. | P2 |
| `register_onboarding_contract` | Registers onboarding data/system. | Onboarding manager/context; registers native onboarding records. | P2 |
| `apply_onboarding_contract` | Applies onboarding state. | Manager/profile state; mutates completion/current-step fields. | P2 |
| `onboarding_accessor_contract` | Context accessor for onboarding manager. | Reads system at registered `onboarding_system_offset`; returns manager pointer. | P2, layout |
| `execute_expression_contract` | Executes native expression. | VM/expression context; returns result/updates graph state. | P2 |
| `execute_profiled_expression_contract` | Executes profiled expression path. | Profiled VM/context; expression state and profiling context are coupled. | P2 |
| `trigger_online_contract` | Triggers native online transition. | Reads backend service/state and invokes online action when available. ReSkate generally runs offline, so null/backend behavior matters. | P2 |
| `trigger_session_contract` | Triggers native session transition. | Session manager/state input; mutates native session lifecycle. | P2 |
| `has_entitlement_contract` | Native entitlement predicate. | Entitlement/member/asset input; returns entitlement state. | P2 |
| `entitlements_ready_contract` | Entitlement-ready flag/predicate. | Reads readiness byte at object +0x220. | P2 |

### Neighborhood and UI model contracts

| Symbol | Contract | ReSkate input/output and side effects | Priority |
|---|---|---|---|
| `neighborhood_update_contract` | Native neighborhood update. | Neighborhood/progression object; reads/writes unlocked/progression state. | P2 |
| `neighborhood_publish_contract` | Publishes neighborhood model. | UI model handle/value; publishes through native model manager. | P2 |
| `neighborhood_ctor_contract` | Constructs neighborhood model. | Destination object; initializes native model fields and references. | P2 |
| `model_manager_contract` | Resolves model manager. | Context/model owner; returns model manager. | P2 |
| `model_destroy_contract` | Destroys model. | Model/allocator; releases fields and native model ownership. | P2 |
| `model_create_contract` | Creates model. | Manager/type/allocator; returns initialized model handle/object. | P2 |
| `model_value_contract` | Reads/writes model value. | Model/field/value data; type and byte width are required. | P2 |
| `model_find_contract` | Model lookup alias to engine::model_find. | Model manager and identity; returns model. | P2 |
| `model_lock_contract` | Locks model. | Model manager/record; increments/acquires native lock. | P2 |
| `model_unlock_contract` | Unlocks model. | Matching model lock; releases/decrements lock. Must pair with lock on all exits. | P2 |

### Cosmetic catalog and recipe path

| Symbol | Contract | ReSkate input/output and side effects | Priority |
|---|---|---|---|
| `load_cosmetic_contract` | Loads cosmetic/profile item. | Native cosmetic/profile object; reads serialized cosmetic state and populates item. | P2 |
| `save_cosmetic_contract` | Saves cosmetic/profile item. | Native object/profile; serializes current cosmetic state. | P2 |
| `copy_cosmetic_contract` | Copies cosmetic record. | Source/destination cosmetic records; deep-copy semantics need verification. | P2 |
| `cosmetic_allocate_contract` | Cosmetic allocator thunk. | Routes to allocator implementation; allocator family and alignment matter. | P2 |
| `cosmetic_construct_contract` | Constructs cosmetic object. | Destination + native type/allocator context; initializes ownership. | P2 |
| `cosmetic_decode_contract` | Decodes cosmetic data. | Serialized bytes/record; fills native cosmetic structure. | P2 |
| `cosmetic_create_catalog_contract` | Creates cosmetic catalog. | Manager/catalog context; allocates native catalog and records. | P2 |
| `cosmetic_complete_catalog_contract` | Completes catalog population. | Catalog plus entries; publishes/marks complete. | P2 |
| `cosmetic_ui_ready_contract` | Cosmetic UI readiness. | Checks model/UI manager availability; gates native UI update. | P2 |
| `cosmetic_control_destroy_contract` | Shared UI control destructor. | Uses `engine::ui_control_destroy`; nested vtable/ownership must match. | P2 |
| `cosmetic_control_free_contract` | Frees cosmetic control object. | Control pointer; destroys owned members and allocator block. | P2 |
| `cosmetic_adapter_allocate_contract` | Allocates aligned cosmetic adapter. | Adapter size/alignment and allocator context; returned object used by control path. | P2 |
| `cosmetic_adapter_aligned_contract` | Aligned adapter allocation path. | Same allocator family; alignment is part of ABI. | P2 |
| `cosmetic_adapter_free_contract` | Frees adapter. | Adapter pointer; must match allocator and allocation mode. | P2 |
| `cosmetic_populate_inventory_contract` | Populates native inventory. | Manager/catalog plus local inventory; fills entries and state consumed by UI. | P2 |

### Card recipe and player card

| Symbol | Contract | ReSkate input/output and side effects | Priority |
|---|---|---|---|
| `card_recipe_ctor_contract` | Constructs card recipe. | Destination recipe; initializes nested native fields. | P2 |
| `card_recipe_destroy_contract` | Destroys card recipe. | Releases nested recipe references/strings. | P2 |
| `card_recipe_get_contract` | Reads recipe slot/value. | Recipe + slot/index; returns or fills recipe value. | P2 |
| `card_recipe_set_contract` | Writes recipe slot/value. | Recipe + slot/index + value; mutates recipe and dirty state. | P2 |
| `card_recipe_ready_contract` | Recipe readiness predicate. | Reads ready flag/asset availability. | P2 |
| `card_info_ctor_contract` | Player-card info constructor alias to engine helper. | Initializes native card info object. | P2 |
| `card_model_field_contract` | Resolves card model field. | Model/field hash; returns field descriptor/offset. | P2 |
| `card_local_info_contract` | Gets local player card info. | Reads local player manager/profile; returns local card object. | P2 |

### Native news/MOTD path

| Symbol | Contract | ReSkate input/output and side effects | Priority |
|---|---|---|---|
| `news_first_contract` | Retrieves first native news item. | News list/context; returns first item or null. | P3 |
| `news_list_contract` | Retrieves news list. | Owner/catalog context; returns native list/array. | P3 |
| `news_ctor_contract` | Constructs Motd/news object. | Destination object; initializes fields including native subobjects. | P3 |
| `news_destroy_contract` | Destroys news object. | Releases nested strings/arrays/references. | P3 |
| `news_array_ctor_contract` | Constructs native news array. | Destination array; initializes allocator/header. | P3 |
| `news_array_destroy_contract` | Destroys native news array. | Frees elements and backing allocation. | P3 |
| `news_append_contract` | Appends news item. | Array + item; may grow and move storage. | P3 |
| `news_assign_contract` | Assigns news text. | Native string destination and source chars; uses engine string assign. | P3 |
| `rip_score_context_contract` | Retrieves RIP score context. | Context/profile; returns score/progression context. | P2 |
| `news_subscribe_contract` | Subscribes to news updates. | Service/listener/delegate; callback lifetime and unsubscription are important. | P3 |
| `news_delegate_copy_contract` | Copies delegate for news subscription. | Native delegate copy semantics. | P3 |
| `news_delegate_destroy_contract` | Destroys news delegate. | Releases delegate capture/refcount. | P3 |
| `news_invoke_contract` | Invokes news delegate. | Delegate + callback payload. | P3 |
| `news_reference_contract` | Assigns future/reference for news response. | Shared state pointer; increments reference count and stores pointer in destination. | P3 |
| `object_subscribe_contract` | Subscribes to object updates. | Service/listener/delegate; callback lifetime. | P2 |
| `object_allocate_contract` | Allocates object data. | Size/alignment/allocator parameters; returns native block. | P2 |
| `object_ctor_contract` | Constructs object record. | Destination object; initializes type/ref fields. | P2 |
| `object_register_contract` | Registers object. | Manager/object key; inserts into native registry. | P2 |
| `object_parse_contract` | Parses object data. | Native parser inputs and destination; exact serialization structure needs caller trace. | P2 |
| `object_release_contract` | Releases object reference. | Native object/ref pointer; decrements reference and frees when appropriate. | P2 |

### Build Kit text/settings helpers

| Symbol | Contract | Inputs / outputs | Priority |
|---|---|---|---|
| `buildkit_text_exists_contract` | Checks localized/buildkit text existence. | Text hash/key; returns existence. | P2 |
| `buildkit_settings_lookup_contract` | Alias to engine settings lookup. | Manager + descriptor; returns settings object. | P2 |
| `buildkit_grabber_settings_contract` | Reads Build Kit grabber settings. | Settings object; reads fields including float/vector settings and flags. | P2 |
| `buildkit_text_translate_contract` | Translates Build Kit text. | Text key/source and output buffer/string; native localization database and ownership apply. | P2 |

### Challenge request and local challenge lifecycle

| Symbol | Contract | ReSkate input/output and side effects | Priority |
|---|---|---|---|
| `challenge_request_create_contract` | Creates native challenge request. | Request owner/context and request parameters; allocates request object. | P1/P2 |
| `challenge_begin_contract` | Begins challenge. | Challenge/activity context; advances native active state and dispatches start behavior. | P1 |
| `challenge_end_contract` | Ends challenge. | Challenge/activity context and result; updates terminal state. | P1 |
| `challenge_available_contract` | Availability predicate. | Challenge ID/context; returns available state. | P2 |
| `challenge_select_contract` | Selects challenge. | Challenge/catalog context; updates selection. | P2 |
| `challenge_lookup_contract` | Looks up challenge. | Challenge identity/key; returns native record. | P2 |
| `challenge_end_invoke_contract` | Invokes end callback. | Delegate/result; callback and completion ordering matter. | P2 |
| `challenge_error_contract` | Constructs/returns challenge error. | Native error payload; must use engine error/string layout. | P2 |
| `challenge_insert_contract` | Inserts challenge record. | Catalog/index and record; mutates native collection. | P2 |
| `challenge_dispatch_contract` | Dispatches challenge event. | Event owner/payload; listeners receive native event. | P2 |

### Local placement networking

| Symbol | Contract | Inputs / outputs / side effects | Priority |
|---|---|---|---|
| `placement_server_create_contract` | Creates server-side placement. | Native placement request/owner/context; allocates authoritative placement state. | P1 |
| `placement_message_contract` | Constructs placement network message. | Destination message and placement fields; native message layout and destructor required. | P1 |
| `placement_apply_contract` | Applies placement state. | Placement object/message; mutates entity/body state. | P1 |
| `placement_construct_contract` | Constructs placement object. | Destination + context; initializes nested members. | P1 |
| `placement_destroy_contract` | Destroys placement object. | Releases nested message/string/entity references. | P1 |
| `placement_resolve_contract` | Resolves placement identity. | Placement key/ID/context; returns live placement or null. | P1 |
| `placement_valid_contract` | Validity predicate. | Placement/body handle; returns validity with generation checks. | P1 |
| `placement_query_contract` | Query-context constructor alias. | Initializes native physics/entity query context. | P1 |
| `placement_pose_contract` | Retrieves placement pose. | Placement/entity/body; fills transform/pose. | P1 |
| `placement_context_contract` | Current context alias. | Returns current thread's game context. | P1 |
| `placement_accessor_contract` | Accesses placement context/system. | Reads registered global/context accessor. | P1 |
| `placement_connection_contract` | Resolves placement connection. | Reads connection/manager vtable path. | P1 |
| `placement_channel_contract` | Reads placement channel. | Returns channel pointer/ID used by message send. | P1 |
| `placement_delete_ctor_contract` | Constructs delete message. | Initializes native delete message. | P1 |
| `placement_message_destroy_contract` | Destroys placement message. | Releases message-owned fields. | P1 |
| `placement_send_contract` | Sends placement message. | Reads connection at placement +0x45d0 and calls vtable +0xc0; message/channel state must be valid. | P1 |
| `placement_teleport_contract` | Teleports placement/player. | Placement/target transform/context; updates native world position. | P1 |
| `placement_transition_contract` | Handles placement transition. | Transition state/context; updates or clears placement state across map/session transition. | P1 |

### Typed user-value service and applied gameplay cache

| Symbol | Contract | Inputs / outputs / side effects | Priority |
|---|---|---|---|
| `user_get_bool_contract` | Native user-value bool getter. | Service/object + key; returns/writes bool result. | P2 |
| `user_get_number_contract` | Native numeric getter. | Service/object + key; returns float/number. | P2 |
| `user_get_integer_contract` | Native integer getter. | Service/object + key; returns integer. | P2 |
| `user_get_string_contract` | Native string getter. | Service/object + key; returns native string or writes output; ownership important. | P2 |
| `user_set_bool_contract` | Native user-value bool setter. | Service/object + key + bool; writes value. | P2 |
| `user_set_number_contract` | Native numeric setter. | Service/object + key + number. | P2 |
| `user_set_integer_contract` | Native integer setter. | Service/object + key + integer. | P2 |
| `user_set_text_contract` | Native text setter variant. | Service/object + key + text; string ownership matters. | P2 |
| `user_set_string_contract` | Native string setter. | Service/object + key + native string. | P2 |
| `user_success_callback_contract` | User-value success continuation. | Native callback payload; completes request and releases captured state. | P2 |
| `user_error_callback_contract` | User-value error continuation. | Native error payload; completes request and propagates failure. | P2 |
| `gameplay_get_bool_contract` | Applied gameplay-cache bool getter. | Reads cached value, separate from backend user-value service. | P1/P2 |
| `gameplay_get_number_contract` | Applied gameplay-cache number getter. | Reads cached numeric value. | P1/P2 |
| `gameplay_get_integer_contract` | Applied gameplay-cache integer getter. | Reads cached integer. | P1/P2 |
| `gameplay_get_string_contract` | Applied gameplay-cache string getter. | Reads cached string; native string lifetime. | P1/P2 |
| `gameplay_set_bool_contract` | Applied gameplay-cache bool setter. | Writes cached bool. | P1/P2 |
| `gameplay_set_number_contract` | Applied gameplay-cache number setter. | Writes cached numeric value. | P1/P2 |
| `gameplay_set_integer_contract` | Applied gameplay-cache integer setter. | Writes cached integer. | P1/P2 |
| `gameplay_set_string_contract` | Applied gameplay-cache string setter. | Writes cached string; allocator and ownership. | P1/P2 |
| `native_setting_get_contract` | Native settings object getter. | Typed settings manager/key lookup; returns native value/object. | P1 |
| `native_setting_set_contract` | Native settings object setter. | Writes typed setting and may notify listeners/dirty state. | P1 |
| `play_event_manager_vtable` | Native play-event history manager vtable. | Data identity used by profile play-event load/ready/send path. | P2 |
| `onboarding_system_offset` | Registered onboarding manager offset in level context. | Context accessor reads this system offset. | P2, layout |

---

## 6. `no_bail.h` — skater physics, placement and animation behavior

This is a coupled gameplay patch family. The core behavior spans multiple producers and consumers. Finding only `record_bail_cause_contract` does not recover the complete behavior because several optimized cause producers are inlined.

| Symbol | Contract | ReSkate sends / reads / changes | Priority |
|---|---|---|---|
| `teleport_on_board_option` | Skater-component arrival preference byte at +0xc0. | Native copy writes on-board, stance and lip-mount bytes at +0xc0..+0xc2; ReSkate changes only on-board byte. | P2, layout |
| `record_bail_cause_contract` | Bail cause collector: `void(collector, i32 reason, float magnitude)`. | Writes cause byte +0x28, XMM2 magnitude +0x4c, increments count +0xe0. Both wipeout and recoverable-runout predicates consume it. | P1, ABI described |
| `choose_physics_state_contract` | Physics-state selector. | Receives selector/current state; WipeoutGround (300) is remapped by caller to Offboard (504). ReSkate filters 300 before remap, while 504 must remain allowed because it also means ordinary walking. | P1 |
| `board_placement_entry_contract` | Board placement entry (Dark Pop unpatch). | `this` placement object (+0x1c0 physics context); rdx arg (+0x54 flag, +0x55 side). Entry tests context +0x13d0 bits 29/30 (0x60000000). | P1 |
| `board_placement_update_contract` | Per-tick board-placement hold update. | `this` placement object; fields +0x29a hold active, +0x29b applied, +0x220 blend, +0x234 timer, +0x23c bone; rdx update arg. | P1 |
| `board_placement_apply_contract` | Per-tick placement apply. | Receives placement object and update argument; applies pose/placement while hold active. | P1 |
| `foot_attach_regular_contract` | Regular foot-attach request setter. | `this` skater; edx mode. Writes mode to skater +0x2f10 object fields +0x1c80/+0x1c84/+0x1c88, bone 0x13; sets dirty +0x1c94. | P1 |
| `foot_attach_switch_contract` | Switch foot-attach request setter. | Same ABI; writes +0x1c70/+0x1c74/+0x1c78, bone 0x0f; sets dirty flag. | P1 |
| `footplant_tick_contract` | State 601 footplant controller tick. | `this` placement object at state +0x48; +0x1c0 context, +0x1c8 skater, +0x228 timer, +0x29c ticked. rdx output has pos +0, vel +0x10, point +0x20, jump +0x30, flag +0x31, valid +0x32. | P1 |
| `skater_mover_contract` | Skater mover. | ABI: rcx skater rig, rdx physics context, r8 input (position +0, direction vectors +0x10/+0x20, byte +0x30), r9b flag. Writes rig root +0x49e0..+0x4a10, world transform context +0/skater +0x56e0, sets context +0x13c4 bit 14, poses rig/bodies. | P1 |
| `bail_animation_caller_contract` | Caller-side animation request transfer. | Verifies caller's request/flag export sequence; not interchangeable with publisher function. | P1, paired evidence |
| `bail_state_caller_contract` | Caller-side state selection transfer. | Verifies native state selector/caller ordering and state fields. | P1, paired evidence |
| `reset_bail_causes_rva` / `reset_bail_causes_code` | Complete native cause reset leaf. | Clears 35 cause bytes, 35 magnitudes and count; leaves owner/config pointers and other collector fields unchanged. No external calls. | P1, patch/code contract |
| `bail_skeleton_contract` | Post-physics skeleton response: `(rig, float dt, bool wipeout)`. | Consumes explicit request at context +0x13c4 bit 15 and independent request at +0x13d4 bit 27. | P1 |
| `bail_publish_contract` | Animation publisher. | Receives physics core; exports animation requests from context flags. | P1 |
| `bail_consumer_contracts` | Five native consumers of bail/animation request flags. | Verifies skeleton request test/ABI and animation exports at +0x87 and +0x8d; leaves separate recover/runout output at +0x9e unchanged. | P1, patch group |
| `body_contact_contracts` | Three body-contact transfer sites. | Collision processing sets +0x10fe for configured bones hitting solid object; rig publisher exports contacts +0xfb8; imported into context +0x13c4 bit 22 and animation contact context +0xd6. Recent-contact latch +0x2c3 remains native. | P1, patch group |
| `body_contact_output_offset` | Contact output at +0xfb8. | Source field offset. | P1, layout |
| `body_contact_context_offset` | Physics context flags at +0x13c4. | Destination field offset. | P1, layout |
| `body_contact_context_mask` | Bit 22 mask 0x400000. | Marks imported body contact. | P1, layout |
| `animation_request_offset` | Request field at +0x13d4. | Animation request byte/word location. | P1, layout |
| `animation_request_mask` | Bit 27 mask 0x08000000. | Independent animation request. | P1, layout |
| `ImpactBail` / `impact_bail_calls` | Collision producer call-site descriptors. | Three call sites set context +0x13c4 bit 15 before recording cause; cancel request at these sites only, preserving unrelated flags. | P1, patch group |
| `impact_request_offset` | Context +0x13c4. | Request flag field. | P1 |
| `impact_request_mask` | Bit 15 mask 0x8000. | Explicit impact request. | P1 |
| `wipeout_physics_state` | State value 300. | WipeoutGround selector value; filter before caller remaps to 504. | P1, constant |
| `offboard_physics_state` | State value 504. | Offboard/walking selector value; must not be blocked. | P1, constant |
| `mount_request_mask` | Context +0x13d4 bit 7 (0x80). | Mount request; bit 8 is dismount. Throwdown waiting state can drop mount request before selector reads it. | P1 |
| `mount_request_test_contract` | Native mount-request test. | Reads mount bit before state selection; patch/adapter must preserve bit 8 dismount behavior. | P1 |
| `bail_core_vtable` | Bail/physics core vtable. | Data identity for native core dispatch. | P1 |
| `recovery_predicate` | Native recovery/stumble predicate. | Reference-only; its result is exported to animation at +0x9e. ReSkate explicitly does not force this predicate. | P1, reference |
| `recovery_animation_output` | Core +0x3b8 -> trick state +0x38. | Path to animation output. | P1, layout |
| `recovery_animation_flag` | Animation output byte +0x9e. | Native recover/runout flag. | P1, layout |

**Non-negotiable dependency cluster:** state selector, context flags, bail collector, inlined cause producers, body-contact transfers, skeleton response and animation publisher must be considered together. A candidate that matches the collector but not the flag consumers can still produce bails or corrupt animation state.

---

## 7. `park_editor.h` — Build Kit movement, picking and surface queries

The header stores arrays of fingerprints. Expand each array into separate candidate records but retain the array name as a group ID.

| Group / symbol | Member count | ReSkate input/output and contract | Priority |
|---|---:|---|---|
| `movement_contracts` | 3 | Native dropper movement (0x7c63f0) uses physics component query, physics body setter and transform setter to update an existing object. Inputs include entity/body identity and updated pose; transform setter's notification source is separately fixed. | P1/P2 |
| `transform_notification_source` | 1 constant | Native transform setter's change-notification source pointer/value. This is data identity, not a function. | P2 |
| `highlight_contracts` | 3 | Build Kit color-override scope: query/constructor, finish/destructor and settings-manager lookup. Scope lifetime must bracket highlighted draw/update and release on every exit. | P2 |
| `picking_contracts` | 6 | Hit body, body-valid check, body-to-entity resolution, entity mapping manager, mapped entity lookup and Build Kit component query. Inputs begin with physics hit/body identity; output is the selected entity/component. | P1/P2 |
| `surface_contracts` | 4 | Surface probe uses physics world, camera ray query, scope allocator and result release. Camera ray query (0x26afdc0) and dropper sphere query (0x26c0fd0) share implementation 0x2664c00 and collision filter; cleanup functions 0x7cc4f0 and 0x175fed0 release results. | P1/P2 |
| `camera_query_filter` | 1 constant | Filter 0x40080303 copied into ray request +0x20; only tests shapes whose category mask intersects. | P2 |
| `terrain_query_filter` | 1 constant | Filter 0x1, serialized Terrain preset. Does not prove a returned hit is terrain/static/actor-free; actual live shape category assignment is unknown. | P2 |
| `all_query_filter` | 1 constant | 0xffffffff diagnostic filter; includes actor shapes whose categories are nonzero. Not safe as a "no actor" filter. | P2 |
| `category_skateboard_collision_helper` | 1 constant | Category bit 31, carried by SkateboardCollisionHelper template. | P2 |
| `named_actor_categories` | 1 mask | Mask assembled from named character/ragdoll/proxy/skateboard/skater categories plus bit 31. These are template definitions, not proof of live shape assignments. | P2 |

**Important negative evidence:** the serialized physics category presets do not establish which live shapes carry each bit. Camera and Terrain masks do not intersect the named actor mask, but that does not prove every actor-free object has a category or that every ramp/prop carries Terrain. A new-build matcher must find both the query implementation and the runtime shape/category data before claiming equivalent picking semantics.

---

## 8. `native_throwdowns.h` — activity, leaderboard, player and challenge APIs

This header mixes hooks, helper functions, globals, graph-native replacements, and native array/string constructors. Keep those roles distinct.

### Hooked throwdown functions

| Symbol | Contract | ReSkate sends / receives | Priority |
|---|---|---|---|
| `player_in_activity` | GetPlayerIsInActivity expression native (hash 6dae8d15). | Answers whether player is in activity; same implementation is used by typed fast thunk and inlined activity pause-menu calls. | P2 |
| `mode_available` | Tests whether a throwdown mode exists in backend catalogue. | Mode identity in, bool out. Offline mode/catalogue state must be represented by ReSkate. | P2 |
| `player_info` | GetUIPlayerInfoByPlayerId expression native. | Player ID in, UIPlayerInfo/model ref out. | P2 |
| `service_player_info` | UIPlayerService PlayerId lookup used by leaderboard rows. | Service + player ID; returns player info. ReSkate may supply relay/local player info when native service has none. | P2 |
| `send_event` | SendNetworkedEvent. | Activity/event payload and context; sends native event through activity/session networking. | P1/P2 |
| `parameters` | Requests throwdown mode parameters from throwdowns_v1. | Native async request/delegate; response flattener creates array mirrored by ReSkate. | P2 |
| `others_throwdowns_allowed` | Throwdowner client system gate. | Reads system +0x100 and backend answer +0xfa; true unless system says gated and backend response denies. Offline response never arrives, so ReSkate must account for this. | P2 |

### Backend, delegate, array and string helpers

| Symbol | Contract | Inputs / outputs / side effects | Priority |
|---|---|---|---|
| `container_lookup` | Finds registered throwdowns_v1 backend container. | Backend services/container key; returns container pointer or null. | P2 |
| `string_assign_chars` | Assigns character range to native string. | String destination, chars, length; native allocator/ownership. | P2 |
| `array_release` | Releases mode parameter array. | Native array wrapper; releases elements/backing block. | P2 |
| `delegate_copy_prefix` | Fingerprint for engine delegate copy. | Shared helper; see engine::delegate_copy. | P2 |
| `delegate_destroy_prefix` | Fingerprint for engine delegate destroy. | Shared helper; see engine::delegate_destroy. | P2 |
| `delegate_invoke_prefix` | Fingerprint for engine delegate invoke. | Shared helper; see engine::delegate_invoke. | P2 |
| `empty_array_prefix` | Fingerprint for native empty-array sentinel. | Sentinel is returned by party::empty_array. | P2 |
| `array_allocate_prefix` | Fingerprint for native array allocator. | Uses engine/party array allocator; wrapper/header ownership required. | P2 |
| `array_allocate` | Alias to engine array allocator. | Wrapper, byte size, alignment, allocator domain. | P2 |
| `empty_string` | Engine empty string sentinel. | Pointer identity used when a native String should be empty; do not substitute null. | P2 |
| `string_destroy_prefix` | Fingerprint for native string destroy. | Must release non-empty owned string, preserve empty sentinel. | P2 |
| `string_assign` / `string_assign_prefix` | Engine string assign. | Destination native String, source chars and length. | P2 |
| `type_construct` / `type_construct_prefix` | Constructs data-defined type instance. | TypeInfo object + event allocator; returns/initializes instance. | P2 |
| `type_destroy` / `type_destroy_prefix` | Destroys data-defined type instance. | TypeInfo + allocator + instance; releases nested fields. | P2 |
| `event_allocator` | Native event allocator pointer. | Allocator passed to type construct/destroy. | P2 |

### Player identity, realm and world globals

| Symbol | Contract | ReSkate reads / writes | Priority |
|---|---|---|---|
| `player_identity_offset` | Registration offset of player identity base class. | Converts player object to identity base. | P1/P2 |
| `player_id_virtual` | Player virtual GetPlayerId at vtable +0x10. | Implementation reads [player +0x48] and returns u32. | P2 |
| `player_id_virtual_prefix` | Four-byte code contract `mov eax,[rcx+0x48]; ret`. | Exact minimal getter behavior. | P2 |
| `realm_context` | GetContextForRealm: `void(context* out, u32 realm)`. | Writes context for requested realm; same function as local_placements::realm_context. | P1 |
| `realm_context_prefix` | Entry validation bytes. | Guard for realm context helper. | P1 |
| `client_player_vars_offset` | Registration offset to client player vars. | Used to convert local player to client player vars as native activity helpers do. | P2 |
| `local_player_base_offset` | Local player base registration offset. | Used to reach local player from context/manager. | P2 |
| `entity_world_manager` | Entity world manager pointer; world pages at +0x70. | Reads manager/world pages for throwdown flag bubbles. | P2 |
| `map_object_store_vtable` | Component store vtable containing MapObjectComponents. | Data identity for flag bubble component store. | P2 |

### Participant, player lookup and challenge functions

| Symbol | Contract | Inputs / outputs / side effects | Priority |
|---|---|---|---|
| `participants_changed` | Posts MPParticipantsChanged(activity, added u32 vector*, removed u32 vector*). | Server calls on every participant change; client calls after diffing replicated list. ReSkate lab uses this to notify subscribers. | P2 |
| `participants_changed_prefix` | Entry validation bytes. | Guard for event dispatch. | P2 |
| `player_name` | GetPlayerName expression native: `(const PlayerRef*, String* out)`. | Writes output String; if PlayerRef null, leaves output unwritten. Caller must initialize/handle this. | P2 |
| `player_name_prefix` | Entry validation bytes. | Guard. | P2 |
| `find_player_by_id` | FindPlayerById: `PlayerRef*(PlayerRef* out, u32 id)`. | Writes zero/null when no player exists. | P2 |
| `find_player_by_id_prefix` | Entry validation bytes. | Guard. | P2 |
| `start_challenge` | Coordinator StartChallenge. | ABI: (coordinator, String* SeriesId, u32Array* participants, String* Id, bool isCoop, u64 group, u32 host, bool available, bool contest). Caller owns/frees participant array. | P1/P2 |
| `start_challenge_prefix` | Entry validation bytes. | Guard. | P2 |
| `array_append_u32` | Appends u32 to native array. | (u32Array*, u32) -> appended slot; updates native count/capacity. | P2 |
| `array_append_u32_prefix` | Entry validation bytes. | Guard. | P2 |
| `challenge_start_request` | Client-side solo challenge request. | (coordinator unused, const String* SeriesId, const String* Id, bool contest); client realm only; sends RequestStartSoloChallenge event 3102c715 with ID, payload, series ID, availability result and contest. | P1/P2 |
| `challenge_start_request_prefix` | Entry validation bytes. | Guard. | P2 |
| `find_player_by_name` | FindPlayerByName: (Address* out player ref, const String* name). | Writes player ref or null. | P2 |
| `find_player_by_name_prefix` | Entry validation bytes. | Guard. | P2 |
| `player_persona_id` | GetPlayerPersonaId: (const Address* player ref, u64* out). | Writes persona ID; leaves output unchanged if no player. | P2 |
| `player_persona_id_prefix` | Entry validation bytes. | Guard. | P2 |
| `player_id_by_name` | GetPlayerIdByName expression native. | u32 (const String* name); server handlers use it to identify event sender. | P2 |
| `player_id_by_name_prefix` | Entry validation bytes. | Guard. | P2 |
| `player_id` | GetPlayerId expression native. | u32 (const PlayerRef*); used after FindPlayerByName. | P2 |
| `player_id_prefix` | Entry validation bytes. | Guard. | P2 |
| `order_participants` | Orders replicated participant list to supplied u32 array order. | (participants component*, const u32 array*); used to set turn order. | P2 |
| `order_participants_prefix` | Entry validation bytes. | Guard. | P2 |
| `set_participants` | SetMpActivityParticipants. | (activity component*, const u32 array*); server realm; queue order becomes turn order. | P2 |
| `set_participants_prefix` | Entry validation bytes. | Guard. | P2 |

### Leaderboards and component access

| Symbol | Contract | Inputs / outputs / side effects | Priority |
|---|---|---|---|
| `leaderboard_score` | SetLeaderboardPlayerScore expression native. | (Leaderboard*, bool, u32 playerId, float score); client realm; sends row to server. | P2 |
| `leaderboard_score_prefix` | Entry validation bytes. | Guard. | P2 |
| `component_lookup` | ECS component lookup. | (entity value, const TypeInfo*) -> component pointer. Used to find leaderboard manager on leaderboard +0x58. | P2 |
| `component_lookup_prefix` | Entry validation bytes. | Guard. | P2 |
| `leaderboard_manager_type` | TypeInfo identity for activity leaderboard manager. | Used with component lookup. | P2 |
| `leaderboard_event_handle` | Reads event handle from leaderboard manager. | u32(manager*); event handle is carried with every row write and checked by server. | P2 |
| `leaderboard_event_handle_prefix` | Entry validation bytes. | Guard. | P2 |
| `leaderboard_write` | Writes leaderboard row. | (manager*, const u64* leaderboard key, bool add, u32 playerId, float score); manager stores leaderboard key at +0x78..+0x80; server receives key index. | P2 |
| `leaderboard_write_prefix` | Entry validation bytes. | Guard. | P2 |

### Graph boolean helpers and integer parsing

| Symbol | Contract | Inputs / outputs / side effects | Priority |
|---|---|---|---|
| `bool_and` | Graph boolean AND. | (const bool* a, const bool* b, bool* out); writes output. Graphs cache function pointers at load, so replacement must preserve target address semantics. | P2 |
| `bool_and_prefix` | Entry validation bytes. | Guard. | P2 |
| `bool_or` | Graph boolean OR. | Same pointer-based ABI, writes output. | P2 |
| `bool_or_prefix` | Entry validation bytes. | Guard. | P2 |
| `bool_not` | Graph boolean NOT. | (const bool* a, bool* out). | P2 |
| `bool_not_prefix` | Entry validation bytes. | Guard. | P2 |
| `parse_int` | ParseInt expression native. | `int(const char* const* text)`; reads string pointer then jumps to atoi in retail. Both generic and fast native entries use it. ReSkate's grouped-number hook changes parsing of grouped scores. | P2 |
| `parse_int_prefix` | Entry validation bytes. | Guard. | P2 |

### Debug variable and native event type identities

| Symbol | Contract | ReSkate-side use | Priority |
|---|---|---|---|
| `debug_variable_row` | Native developer-variable table row (hash 0xbc999b66). | ReSkate patches row's function pointer to its implementation. This is a table slot mutation, not a function hook. | P3 |
| `debug_variable_stub` | Shared "return default" stub target. | Original table target; shared stub must not be globally detoured because other rows use it. | P3 |
| `event_allocator` | Native event allocator. | Passed to TypeInfo constructor/destructor for event payload instances. | P2 |
| `empty_string` | Native empty String sentinel. | String value is one pointer; empty points here; destroy must not free sentinel. | P2 |
| `string_destroy_prefix` | String destructor bytes. | Verifies correct string ownership path. | P2 |
| `type_construct` / `type_construct_prefix` | TypeInfo-based construction. | (typeObject, allocator) -> instance. | P2 |
| `type_destroy` / `type_destroy_prefix` | TypeInfo-based destruction. | (typeObject, allocator, instance). | P2 |

**Important dependency edges:** throwdown hooks depend on native delegate/array/string helpers and player identity/context functions. A recovered `player_info` function does not establish that the native UI can display relay players unless `service_player_info`, local player identity, model TypeInfos and model ownership also match. Similarly, a matching `start_challenge` entry is incomplete without the participant array layout and ownership rules.

---

## Cross-header dependency graph

| Shared primitive / identity | Direct dependent contracts |
|---|---|
| `engine::current_context`, context realm/type offsets, TLS | Runtime, profile, placement, native party, throwdowns, local settings |
| `engine::array_allocate`, native array header/sentinel/free | Native party requests, throwdown parameter/participant arrays, profile challenge/catalog arrays |
| `engine::delegate_copy/destroy/invoke` | Party async requests, profile news/user-value callbacks, throwdown parameter/challenge callbacks |
| `engine::string_assign/native_text_release/empty_cstring` | Party result/event payloads, profile, player cards, throwdown names/events |
| `engine::construct_error/complete_promise/assign_future` | Entitlement and profile async flows |
| UI model manager/TypeInfos/lock/unlock | Party roster, profile neighborhood/cosmetics/card/news, throwdown leaderboard |
| Client vtable + client state + level description layout | Runtime tick, transition, local level request, sublevel listener |
| Physics context offsets + skater rig layout | No Bail, offboard flight, skater state, placement, park editor |

## Scanner representation recommendation

Represent each symbol as an individual dependency record, with a parent group for coupled arrays/patches:

- `id`: stable key such as `no_bail.record_bail_cause_contract`.
- `kind`: `function`, `hook`, `callsite`, `vtable`, `typeinfo`, `global`, `field_offset`, `constant`, `patch_group`, `data`.
- `priority`: P0-P3.
- `contract_status`: `verified`, `header_described`, `callsite_unverified`, `layout_only`, `patch_group`.
- `abi`: known parameter roles/types/return; leave unknowns explicitly null rather than guessing.
- `reads`, `writes`, `side_effects`, `ownership`, `thread_realm`.
- `related_symbols`: caller/callee, shared helper, paired constructor/destructor, corresponding vtable/TypeInfo/global.
- `strict_constraints`: exact ABI, argument roles, structure size/layout, side effects.
- `relaxable_constraints`: register/argument reshuffle, extra arguments, changed structure representation, changed implementation.
- `adapter_notes`: how ReSkate could marshal old expected data to the candidate ABI.

For coupled contracts, the scanner must retain group completeness. For example, the No Bail flag-transfer sites, park-editor query+cleanup pairs, party request+delegate+handle flow, and profile model lock/unlock pairs should not be counted as recovered when only one member is found.


### Throwdown hook entry guards

The following prefix symbols are independent validation records paired with their corresponding hook target. They do not add new runtime behavior, but each guards a distinct hook entry and must remain separately addressable in the compatibility inventory:

- `player_in_activity_prefix` -> `player_in_activity`
- `mode_available_prefix` -> `mode_available`
- `player_info_prefix` -> `player_info`
- `service_player_info_prefix` -> `service_player_info`
- `send_event_prefix` -> `send_event`
- `parameters_prefix` -> `parameters`
- `others_throwdowns_allowed_prefix` -> `others_throwdowns_allowed`
- `container_lookup_prefix` -> `container_lookup`
- `string_assign_chars_prefix` -> `string_assign_chars`
- `array_release_prefix` -> `array_release`

Each guard is an entry-byte validation contract. For changed builds, instruction matching should be performed on the target function independently of the guard bytes, then validate the inferred behavior and call-site relationships before accepting a hook.


---

## 9. `loose_files.h` — native VFS overlay

Consumer: `Extension/Assets/loose_files.cpp`.

| Symbol | ReSkate-side contract | Inputs / outputs / side effects | Priority / status |
|---|---|---|---|
| `vfs_open` | Hooked VFS open entry. | ABI is `void*(vfs, unsigned flags, const char* name)`. ReSkate redirects only read requests (flag bit 0 set, write bit 1 clear); writes/saves and recursive overlay reads are forwarded unchanged. For an existing loose path it calls the original open on the mounted overlay VFS and returns the engine's native file object. Missing loose files fall through to the original VFS/path. | P1, ABI and behavior traced |
| `vfs_exists` | Hooked VFS existence query. | ABI is `bool(vfs, const char* name)`. Returns true for a validated loose-file candidate, otherwise forwards to the original VFS. Recursive overlay queries are forwarded to avoid re-entering the overlay. | P1, ABI and behavior traced |
| `create_directory` | Creates the engine directory filesystem. | ABI is `void*(const char* root)`. Root is the executable directory encoded as UTF-8 with a trailing slash. Returned object is an engine-owned filesystem object with an intrusive reference count at +8. | P1, ABI described |
| `vfs_mount` | Mounts directory filesystem into the global VFS. | ABI is `bool(vfs, file_system, const char* mount_point)`; mount point is `/__reskate_loose`. On success the VFS resolves redirected paths through the mounted directory. | P1, ABI described |
| `global_vfs` | Retrieves global VFS. | No arguments; returns null before VFS initialization, in which case mount creation is retried on a later read. | P1, ABI described |
| `mount_reference` | Intrusive reference increment instruction site. | Interlocked increment of the directory object's 32-bit reference count at +8. ReSkate deliberately retains one process-lifetime reference independent of the mount. This is a data/ownership contract, not a callable function. | P1, instruction/data identity |

**Coupled behavior:** the two hooks, directory factory, mount, global VFS, mount point and reference increment form one overlay contract. Replacing only open/exists is insufficient. The native file object must remain engine-created so allocator, refcount, read/seek/close behavior and flags remain compatible. Path normalization is delegated to `initfs::loose_relative_path`; only safe paths beneath the executable root are considered. The overlay is session-gated and does not make writes loose-file-backed.

**Important distinction:** the factory and mount are called directly by ReSkate, while open/exists are detoured and their trampolines retained. A behaviorally equivalent candidate for the factory/mount still needs a compatible filesystem object layout and reference ownership.

## 10. `live_mods.h` — runtime superbundle and TOC mutation

Consumer: `Extension/Assets/live_mods.cpp`.

| Symbol / field | ReSkate-side contract | Inputs / outputs / side effects | Priority / status |
|---|---|---|---|
| `register_superbundle` | Adds a named superbundle to the layer-set registry. | ABI: `void(registry, const char* name)`. The engine retains the name pointer; ReSkate stores names in a process-lifetime deque so reallocation does not invalidate them. Called while holding the layout manager critical section at manager +0x130. | P1, call ABI described |
| `find_superbundle` | Finds a registered superbundle. | ABI: `void*(registry, const char* name, char prefixed)`; ReSkate uses prefixed=0 before/after registration and while locating mounted TOCs. | P1, call ABI described |
| `attach_superbundle` | Associates superbundle with an install chunk. | ABI: `void*(superbundle, unsigned kind, void* chunk, int mode)`; ReSkate calls kind=0 and mode=1. This makes the bundle discoverable through the selected install chunk. Return value is not used as an owned object by the caller. | P1, call ABI described |
| `prepare_toc` | Converts TOC data words to native byte order. | ABI: `void(data)`; mutates the supplied buffer in place. Called on bytes immediately after the 0x22c-byte file header before comparing format and swapping the live TOC backing. Mutates the supplied buffer in place. | P1, call ABI described |
| `manager_lock` | Layout manager critical section offset +0x130. | ReSkate enters it around registry, superbundle and mount-TOC operations. Must be the lock protecting these structures; matching a similarly shaped lock elsewhere is unsafe. | P1, data identity |
| `layers_registry` | Layer set +0x58 -> superbundle registry. | Pointer field used to reach registry from the layout objects captured by the mod-layer hook. | P1, field identity |
| `registry_manifest` | Registry +0x28 -> install manifest. | Pointer field; null/unreadable means no chunk registration can proceed. | P1, field identity |
| `manifest_chunks` | Manifest +0x28 -> begin/end pointer vector. | ReSkate iterates qword pointers, bounded to 1024 entries, and compares each chunk's name. | P1, field/layout identity |
| `chunk_name` | Install chunk +0x100 -> engine string. | ReSkate reads Frostbite small-string representation: inline up to 15 bytes, otherwise pointer selected by bit 0x80 at +0xf. | P1, field/layout identity |
| `superbundle_mounts` | Superbundle +0x30 -> begin/end vector. | ReSkate walks mounted layer pointers while holding manager lock. | P1, field/layout identity |
| `mount_tocs` | Mount +0x50 -> TOC entry vector. | ReSkate selects a TOC whose format matches the merged file and rewrites its backing pointers. | P1, field/layout identity |
| `toc_entry_size` | TOC row size 0x28. | Used to interpret TOC-entry records. | P1, layout constant |
| `toc_entry_file` / `toc_entry_data` | Entry +0x10 file object; +0x18 data pointer. | The data pointer must equal file-object bytes pointer +0x22c before replacement. The two entry qwords at +0x18/+0x20 are both rewritten to the new data address. | P1, layout identity |
| `toc_file_bytes` / `toc_file_size` | File object +0x28 backing bytes; +0x30 and +0x38 size fields. | ReSkate allocates a process-lifetime copy with VirtualAlloc and updates backing pointer and both sizes. Old backing memory is intentionally not freed because native readers may still hold it. | P1, layout/ownership identity |
| `toc_header` | 0x22c-byte TOC file header. | Separates the file header from the TOC data passed to prepare_toc. | P1, layout constant |
| `root_description` | Root LevelDescriptionAsset path. | Asset lookup searches domain owners for this exact asset. | P1, asset identity |
| `description_on_demand` | Root description +0x40 on-demand list pointer. | The pointer targets rows; count is a u32 at pointer-4. ReSkate copies old rows, appends {name pointer, bundle hash, padding}, allocates a replacement array and writes the new pointer. | P1, field/layout identity |
| `on_demand_entry_size` | On-demand row size 0x10. | Each row begins with retained name pointer and u32 case-insensitive djb2 bundle hash. | P1, layout constant |

**Coupled behavior and ownership:** registry registration, chunk attachment, root on-demand registration and TOC replacement are distinct but coordinated operations. ReSkate keeps superbundle and level-name strings alive for the process because native objects retain their pointers. Replacement TOC buffers and root-list allocations are also intentionally not freed. Any candidate mapping must preserve lock coverage, registry/list topology, TOC row layout, format selection, native-endian conversion and pointer lifetime. A function match alone does not recover these object contracts.

**Dependency edge:** `mod_layers::layout_objects()` supplies the manager and layer set; `engine::domain_owners` plus native asset lookup locates the root description. The root on-demand list makes newly registered map bundles discoverable during later level loads.

## 11. `mod_layers.h` — merged Patch/Data layer interception

Consumer: `Extension/Assets/mod_layers.cpp`.

| Symbol | ReSkate-side contract | Inputs / outputs / side effects | Priority / status |
|---|---|---|---|
| `classify` | Native layout.toc path classifier. | ABI: `uint8_t(const char* path)`. ReSkate returns the native Patch kind (2) for paths under `/native_data/Mods/`; all other paths are forwarded to the original classifier. | P1, call ABI traced |
| `load_layer` | Layout manager layer loader. | ABI: `void(self, manager, const char* path)`. ReSkate intercepts the first stock Patch or Data layer, substitutes the merged Mods layout.toc if present, records manager/layer-set objects for live-mod operations, and forwards stock loading when no merged file exists. It ensures the merged layer is loaded once and avoids a second Patch-kind layer. | P1, call ABI traced |
| `load_layer_slot` | Layout manager vtable slot at +0x60. | Validation requires the slot's function pointer to equal `load_layer`. This couples the hook to the expected class/vtable method identity, not merely to matching bytes at the function RVA. | P1, vtable-slot identity |
| `layout_bootstrap` | Startup layer bootstrap. | Performs LCU, Patch and Data loads in that order. ReSkate validates its prefix because the substitution logic relies on the startup order and on Patch being optional while Data is always loaded. | P1, patch/call-order dependency |

**Behavioral constraints:** every mod is merged into one extra Patch-kind layer. Two layers with the same kind can desynchronize bundle reads, so the old Patch directory is not mounted separately once the merged Mods layer replaces it. Classifier and loader hooks are enabled in one Detours transaction before startup. The loader hook's `expanded` state permits only one substitution. A candidate that classifies Mods correctly but changes bootstrap order or the +0x60 virtual dispatch is not compatible.

**Failure behavior:** invalid mod catalog/content is treated as user content failure and startup continues with stock layers; a mismatched native contract prevents installing the hooks and reports a startup error. Keep these distinct in a behavioral port.

## 12. `world_layers.h` — SubWorldReference lifecycle and event clone

Consumer: `Extension/World/local_world_layers.cpp`.

| Symbol / site | ReSkate-side contract | Inputs / outputs / side effects | Priority / status |
|---|---|---|---|
| `sites[0]` | SubWorldReference constructor. | Hook receives the entity being constructed plus native construction arguments; the original constructor is called first. ReSkate then inspects the initialized entity to capture its data, owner, context and bundle identity. | P1, hook role described; exact prototype must be verified at call sites |
| `sites[1]` | SubWorldReference destructor. | Hook removes the entity from seasonal observation and world-layer tracking before forwarding destruction. The flags argument is observed in the consumer hook. | P1, hook role described; exact prototype needs ABI confirmation |
| `sites[2]` | SubWorldReference event handler. | Native handler ABI is described as `void(entity, event*)`. ReSkate observes event identity/state at the native boundary and forwards exactly once. It does not hold its observer lock across native dispatch because dispatch can re-enter. | P1, call ABI described |
| `sites[3]` | Subworld load/unload event clone. | Clone function for a 16-byte event, referenced by event vtable slot +0x18. The clone copies the event into its own arena; it must not retain the caller's event pointer. | P1, data/ownership contract described |
| `subworld_reference_vtable` | SubWorldReference entity vtable. | ReSkate validates entity identity before reading fields at +0x30 (owner), +0x38 (data), +0x80 (handle), +0x148 (flags). | P1, vtable and field-layout identity |
| `subworld_event_vtable` | Subworld load/unload event vtable. | Slot +0x18 is the clone function at sites[3]. | P1, vtable-slot identity |

**Lifecycle/data contract:** captured node identity includes entity, data, context and a generation number. Bundle identifier is read from data +0xd8. The owner/context chain is entity +0x30 -> owner +0x20 -> context. Seasonal-state observation treats a nonzero handle at entity +0x80 as requested and bit 0 of entity +0x148 as loaded. Destruction erases observations before the native object is released. The runtime resets session state on world transitions.

**Unknowns retained:** the header supplies 16-byte prefixes and semantic roles, but this document does not yet claim a fully reconstructed constructor argument layout, destructor flag semantics, event payload schema beyond 16 bytes, or arena allocator ABI. Those require native call-site/callee tracing before an adapter can be written.

## 13. `streaming_reads.h` — staged streaming scatter-copy guard

Consumer: `Extension/World/streaming_copy_guard.cpp`.

| Symbol / field | ReSkate-side contract | Inputs / outputs / side effects | Priority / status |
|---|---|---|---|
| `finish` | End of a staged streaming read. | Hook ABI: `void(scheduler, request)`. ReSkate inspects request flags and scatter-copy descriptors before forwarding once to the original function. Runs on the I/O thread. | P1, hook ABI traced |
| `places_offset` | Request +0x20 pointer to Place array. | ReSkate reads the native array pointer. | P1, field identity |
| `flags_offset` | Request +0x34 u32 flags. | Bits 0 and 1 indicate staged data; bits 2..23 encode number of Place entries. | P1, field/bitfield identity |
| `Place` | Scatter-copy descriptor, size 0x18. | Fields: destination pointer, size, stride, staging-buffer offset, count. The engine copies count pieces of size bytes from staging offset to destination using stride. | P1, structure layout |
| `staging_size` | Staging buffer capacity 0x10000 bytes. | ReSkate validates offset + size * max(count,1) against 64 KiB. | P1, layout constant |
| `staged_flags`, `place_count_shift`, `place_count_mask` | Request flag interpretation. | Exact values are 3, 2 and 0x3fffff. | P1, bitfield constants |

**Side effect:** if a descriptor would read beyond the 64 KiB staging buffer, ReSkate logs it and writes zero to that descriptor's `size`. The native loop still iterates count times but copies zero bytes, preventing an out-of-bounds read while allowing the rest of the request to complete. It does not rewrite the request count or skip the original function.

**Compatibility requirement:** a candidate finish function is not sufficient unless the request layout, staged flag semantics, scatter descriptor layout and copy arithmetic remain equivalent. The guard changes a descriptor in place, so the descriptor must be writable and belong to the request being processed. The original must be called exactly once after validation. The 32-byte fingerprint is only the pinned-build gate, not the semantic match criterion.

## 14. `native_skater.h` — remote skater and skateboard entity/pose integration

Consumer: `Extension/Multiplayer/Remote/native_skater.cpp`.

| Symbol / site | ReSkate-side contract | Inputs / outputs / side effects | Priority / status |
|---|---|---|---|
| `animation_update` | Skater animation component update hook. | Hook receives component and update context. ReSkate forwards native animation evaluation, then applies the latest session pose to the remote skater's native skeleton buffer when the component belongs to the watched peer. | P1, function identity known; exact hook prototype/call order must be verified from remaining consumer code |
| `board_render_pose_prefix` / `board_render_pose_interface_vtable` | Board render-pose interface method identity. | Vtable slot +0x10 must resolve to the native board render-pose method. ReSkate hooks this publication boundary, calls original first, then notifies registered render listeners with animation interface and render data. | P1, vtable/call boundary |
| `submit_render_pose` | Renderer submission thunk. | Signature in header comment: (slot, render_data) -> uintptr_t. It dispatches through renderer interface slot +0xd8. It is used as a reference to establish when render data/bones are consumed, not directly called by the remote-skater adapter. | P1, call relationship described |
| `render_animation_interface_offset` | Animation holder +0xc0. | Evaluated render interface embedded at this offset. ReSkate recognizes a board holder's interface by comparing animation_interface to holder +0xc0. | P1, field/layout identity |
| `board_embedded_holder_offset` | Entity block +0xd10. | Measured location of the embedded animation holder in local/menu board actors. Reverse lookup must validate all ownership links; offset alone is not sufficient. | P1, measured layout, guarded |
| `entity_helper` | Entity helper associated with creation functions. | Header marks it as a verified helper but consumer does not call it directly. Retain as a related symbol, not a direct runtime dependency. | P2, reference identity |
| `set_blueprint_transform` | Updates skateboard blueprint world transform. | ABI used by consumer: `void(transform, matrix)`. ReSkate writes the board's transform before native entity placement to keep render-space transforms coherent. | P1, direct call |
| `blueprint_transform_vtable` | Blueprint transform object vtable. | Validates the board's transform object before mutation; transform object must not alias the local skater's transform. | P1, object identity |
| `place_board` | Native skateboard source placement setter. | ABI used by consumer: `void(entity, matrix)`. Places the board entity and updates rig input through its native call path. | P1, direct call |
| `place_board_rig_call` | Internal call site in place_board. | Validates the expected rig-input update call relationship. | P1, patch/call-site identity |
| `board_world_transform_event` / `board_placement_callback` | Native reference paths. | Event updates blueprint transform before entity placement; callback publishes inverse(parentWorld) * entityWorld to renderer. These are explanatory references rather than direct calls from this adapter. | P1, semantic reference |
| `enable_board_resources` | Enables board holder render resources. | ABI comment: (holder, 1). Native board startup uses the same call; holder vtable slot +0xf8 is involved. | P1, direct/related lifecycle dependency |
| `board_holder_resource_methods` | Board holder resource lifecycle methods. | Vtable slots +0x88 and +0xc8. | P1, vtable method identities |
| `board_startup`, `board_physics_create` | Native startup/physics reference functions. | Reference only; establish when board resources and physics are initialized. | P1/P2, semantic references |
| `initialize_creation_list` | Initializes entities in a blueprint creation list. | Called as part of entity creation lifecycle; prefix is the entry guard. | P1, related function identity |
| `free_creation_page` | Allocator dispatch for creation-list page release. | Header ABI: (list, page, size). Must use compatible allocator/free path. | P1, ownership helper |
| `pose_output_resolve` | Resolves current output pose by signed table index. | Live 395-bone skater uses index -1, selecting table word 18. Reference used by pose layout reconstruction. | P1, data/layout helper |
| `component_cache_callback` | Caches skater component at entity +0x628 after construction. | Reference for entity/component relationship. | P1, field/callback identity |
| `board_from_component` | Follows skater component +0x60 backlink to separate ClientSkateboardEntity. | Reference for ownership graph. | P1, relationship helper |
| `board_animation_holder` | Returns board entity animation holder at +0xf0. | Reference for holder discovery. | P1, field/helper identity |

**Remote-pose behavior:** ReSkate receives normalized transforms from its session layer, places the actor, and writes scale, rotation and position into the native bone array in place. Destination W lanes are deliberately preserved because native board poses can use position.w for non-float metadata. If animation resources are still streaming, pose application returns no buffer and retries later. If the native graph evaluated a fresh pose, ReSkate rewrites it; if the session target revision is unchanged and the same buffer remains, it avoids a redundant write.

**Separate board contract:** the skateboard is a distinct visual entity with its own animation holder and pose buffer. ReSkate validates entity/component/holder ownership links, writes the blueprint transform first, then calls native board placement. At render publication it calls the original render-pose method first, verifies render_data +0x10 is the expected pose buffer, then writes the remote board bones. Board failures are isolated from skater playback so the skater can continue.

**Lifetime and concurrency:** watched entity/component/board pointers are indexed by peer slot and cleared on entity destruction. A generation counter invalidates stale state. Animation application uses a mutex; render publication uses try-lock because native placement can re-enter engine notifications. Native memory reads/writes are guarded against access violations. A candidate match must preserve this phase ordering, ownership validation and pointer lifetime; matching only a pose-writing helper is not enough.

**ABI gaps:** the header contains many reference RVAs, vtables and prefixes, but not all are direct hook targets. The full animation hook and entity-destruction hook prototypes must be checked against the remainder of the consumer and native callers before assigning a strict ABI signature. The pose buffer layout is a separate data contract, not inferred from the function prefix.


## 15. `native_indicators.h` — native HUD nametag and compass providers

Consumer: `Extension/Multiplayer/Hud/native_indicators.cpp`.

| Symbol / identity | ReSkate-side contract | Inputs / outputs / side effects | Priority / status |
|---|---|---|---|
| `hud_tick` | Native HUD indicator presenter tick. | Presenter object is passed as the native receiver. Hook tracks the current presenter while forwarding the original tick; provider creation/removal is coordinated on this native UI lifecycle path. | P1, hook role described; full prototype should be confirmed from hook declaration |
| `provider_remove` | Removes a provider from the global indicator registry. | Registry and Provider pointer; ReSkate intercepts removal to synchronously stop its own provider, flush pending removals, then forwards the native removal. | P1, call boundary described |
| `widget_create` | Creates/caches an indicator widget. | Consumer's wrapper ABI: `Address(cache, result, int id, Address list, Address info, const Callback* before, const Callback* after)`. ReSkate only substitutes an empty callback with a pre-init binding callback for the exact current presenter cache and matching template blueprint; all other calls pass through unchanged. | P1, consumer ABI traced |
| `indicator_register` | Registers Provider with indicator registry. | Called with registry and Provider; the Provider vtable/method table supplies destroy, transform, blueprint, parent, owner, data, enabled, unregister and sort callbacks. | P1, provider contract described |
| `presenter_flush` | Processes pending presenter removals. | Called for each discovered presenter after provider removal so widget teardown occurs on the native UI lifecycle path before borrowed template parents/assets are released. | P1, call ABI used as void(presenter) |
| `property_bind` | Initializes an output property binding on a widget parent. | Consumer call ABI: `void(parent, &initialized, interface_data, hash, type, nullptr)`. Returned initialized slot must equal the independently discovered output property slot. | P1, call ABI traced |
| `class_reference_getter` | Resolves a class-reference value. | Reads storage, clears bit 2, writes the result through output pointer. Used by property setters as a class-asset reference; not the provider's core update loop. | P2, header-described |
| `set_customization_flag` | Shared engine helper to publish a property value. | Called with pointer to initialized property slot, value address and flag 1. The property must be a separate widget output, not an alias of template/source data. | P1, shared helper |
| `model_record` / `model_type` | Native UI model lookup/type helpers. | Used under model manager write lock to validate model handles and TypeInfo. The record is resolved from manager, handle and mode; type lookup uses manager, handle, record and mode. | P1, shared model contract |
| `card_info_construct` | Constructs a native player-info/card record. | Called on an aligned 0x108-byte local buffer before publishing it into the model manager. The native model takes its own published value; local native string storage is released after publication. | P1, ABI/layout traced |
| `native_text_release` | Releases temporary native text/string value. | Called on address of a native string value after publication. Must use the engine string release semantics; not interchangeable with CRT free. | P1, shared ownership helper |
| `presenter_registry` | Global indicator provider registry. | Registry table at +0x20, bucket count +0x28 and element count +0x2c. ReSkate bounds traversal and validates each presenter vtable. | P1, global/layout identity |
| `hud_presenter_vtable` | Presenter object identity. | Used to validate objects found in the presenter registry. | P1, vtable identity |
| `indicator_source_vtable` | Indicator template source identity. | Used to discover native nametag/compass templates and their backing data. | P1, vtable identity |
| `widget_parent_vtable` | Native widget parent identity. | Validates parent objects and independent output property bindings. | P1, vtable identity |
| `compass_icon_type`, `vec3_type`, `user_info_type` | Native property TypeInfos. | Compass icon asset, Vec3 anchor/offset, and user-info field. Property types must match exactly before binding. | P1/P2, TypeInfo identity |

**Provider layout:** ReSkate's Provider begins with a methods pointer, then id at +8 and channel begin/end/capacity at +0x10 onward. It contains allocator, fixed channel storage, borrowed template/source/blueprint/parent/owner/data pointers, transform offset/world matrix, lifecycle flags, widget-parent pointer and peer slot. Static assertions pin key offsets and ProviderMethods size (0x48). This is a ReSkate-owned object passed into the native registry; its lifetime extends through deferred native removal.

**Widget binding behavior:** ReSkate discovers stock indicator source templates, borrows their blueprint/parent/data while they remain loaded, then creates a widget with an independent output property set. Nametag binding creates an independently owned model context that points to the remote player's PlayerInfo record. Compass binding copies selected properties and resolves the party/player compass texture while holding the native texture registry lock; the setter retains the asset before the lock is released. The borrowed template and texture must not be cached beyond their native lifetime.

**Thread/lifecycle constraints:** provider registry operations use a recursive native mutex. Model publication uses the model manager's write lock. Provider stop unregisters and flushes presenters before clearing borrowed pointers. Peer slot state is generation-tracked; idle slots skip presenter work. A candidate must preserve this provider vtable contract, callback injection conditions, native widget property ownership and teardown order.

**ABI gaps:** the header lists function RVAs/prefixes but not C++ prototypes for every hook. The consumer defines the Provider callback ABI and widget wrapper ABI; exact native register/stack prototypes for HUD tick, provider removal, indicator registration and presenter flush should be verified against native callers before creating direct-call adapters.

## 16. `native_player_ui.h` — remote player map-location entries

Consumer: `Extension/Multiplayer/Hud/native_player_ui.cpp`.

| Symbol / field | ReSkate-side contract | Inputs / outputs / side effects | Priority / status |
|---|---|---|---|
| `map_tick` | Native map display interface tick, vtable slot +8. | Hook receives interface address, validates display and interface vtables, updates per-peer location entries, then forwards native tick. | P1, hook ABI traced |
| `ui_copy` | Shared first-256-byte UI move helper. | Hook forwards destination/source unchanged. Only when return address equals the exact map-tick call return does ReSkate append remote location handles to the map render-message list. Other UI copies are untouched. | P1, call-site-gated hook |
| `map_destroy` | Map display destructor. | Hook releases any handles owned for that map object before forwarding destruction. ABI used: `uintptr_t(object, unsigned flags)`. | P1, hook ABI traced |
| `location_shutdown` | Player-location manager shutdown. | Hook releases every peer's map handles and clears per-map records before forwarding shutdown. Consumer ABI: `void(uint8_t unregister)`. | P1, hook ABI traced |
| `location_create` | Creates a player-location entry. | Consumer ABI: `uint16_t(manager, const void* key, const void* Location, uint8_t scope)`. Key is a per-peer 64-bit value; Location contains two projected floats and a field byte. Handles below 2 are treated as failure/reserved. | P1, call ABI traced |
| `location_update` | Updates an existing location entry. | Consumer ABI: `void(manager, uint16_t handle, const void* Location)`. Called when map projection changes. | P1, call ABI traced |
| `location_release` | Releases a location handle. | Consumer ABI: `void(manager, uint16_t handle)`. Called on invisibility, map destruction and manager shutdown. | P1, ownership contract |
| `location_retain` | Retains a location handle. | Consumer ABI: `void(manager, uint16_t handle)`. Called once for each appended render-message reference; native message destruction owns the matching release. | P1, ownership contract |
| `list_append` | Appends a u32 entry to native render-message location array. | Consumer ABI: `uint32_t*(array, 0)`. ReSkate writes the low 16-bit location handle into the returned slot after retaining it. | P1, call ABI traced |
| `map_project` | Projects world position to map coordinates. | Consumer ABI used: `void*(out, world, mapSize, mapOrigin)`; output is two floats. Uses map display size at object +0x5320 and origin at +0x5310. | P1, call ABI traced |
| `copy_call` / `copy_return` | Exact call site and return address for the shared UI copy helper. | The copy hook mutates map-specific message fields only when `_ReturnAddress()` equals copy_return. This avoids corrupting unrelated UI structures that share the helper. | P1, call-site identity |
| `location_manager` | Global pointer to player-location manager. | Must match both the expected manager vtable and current global pointer before handles are updated/released. | P1, global identity |
| `location_manager_vtable` | Location manager vtable. | Slot +0 is retain; slot +0x18 is update. | P1, vtable identity |
| `map_display_vtable` / `map_display_interface_vtable` | Map display object/interface identities. | Interface is embedded at object +0x40; map tick slot +8 points to map_tick. | P1, vtable identity |

**Data and lifetime:** a Location record is two float coordinates, a one-byte fields mask (default 1), and padding. Each peer maintains a map from map-display object to {manager, 16-bit handle}. Projection is refreshed at most every 50 ms. The manager is validated against its global pointer and vtable before use. The render-message location array uses 4-byte entries with the handle in the low half; the high-level array count is read from the word at array-4 and bounded below 128.

**Critical reference counting:** creating a map location establishes the peer's owned handle. Every render message that receives the handle first calls retain; the native message's destruction releases that reference. Releasing the peer's own handle on map invisibility/destruction/shutdown does not substitute for releasing message references. A candidate implementation must preserve this ownership split or it can cause stale handles or premature release.

**Hook coupling:** map tick, shared copy, map destructor and location-manager shutdown are prepared and enabled together in one Detours transaction. The copy hook is safe only with the exact callsite/return-address discriminator. Vtable checks ensure the map display and manager methods still occupy the expected slots.

## 17. `replay_export.h` — replay MP4 encoder configuration and pacing

Consumer: `Extension/Rendering/replay_export.cpp`.

| Symbol / field | ReSkate-side contract | Inputs / outputs / side effects | Priority / status |
|---|---|---|---|
| `submit_frame` | Replay encoder frame submission. | Hook ABI used: `void(encoder, pixels, pitch, unused)`; all four register arguments are forwarded unchanged. For the replay editor's encoder only (vtable identity), ReSkate sets frame-permit byte +0x90 to 1 before forwarding, allowing every rendered frame to be encoded. | P2, hook ABI traced |
| `replay_encoder_vtable` | Replay editor encoder identity. | Prevents frame-permit mutation on other encoder types. | P2, vtable identity |
| `frame_permit` | Encoder +0x90 byte. | World update sets it and encoded frame clears it; submit drops a frame while clear. ReSkate writes 1 for the replay editor encoder. | P2, field/behavior identity |
| `configure_h264` | Media Foundation H.264 encoder setup. | Hook ABI used: `uintptr_t(encoder, a, b, c)`; the remaining three arguments are forwarded unchanged. ReSkate changes encoder parameters before native setup reads them, then returns the native HRESULT-sized result. | P2, hook ABI traced |
| `h264_kilobits` | Encoder +0x70 average bitrate in kbit/s. | ReSkate computes desired bitrate from user quality, dimensions and FPS, then writes it. | P2, field identity |
| `h264_rate_mode` | Encoder +0x78 rate-control mode. | 0=CBR, 1=peak-constrained VBR, 3=quality mode. | P2, field/enum identity |
| `h264_quality` | Encoder +0x7c quality 0..100. | Updated only for mode 3. | P2, field identity |
| `h264_peak` | Encoder +0x80 peak bitrate. | For mode 1, scaled by desired/old average bitrate to preserve its ratio. | P2, field identity |
| `h264_width` / `h264_height` | Encoder +0x88/+0x8c dimensions. | Read-only inputs to bitrate calculation and diagnostics. | P2, field identity |
| `h264_fps` | Encoder +0x90 frame rate float. | Read-only input to bitrate calculation and diagnostics. | P2, field identity |

**External Windows API hook:** the consumer also detours Media Foundation `MFTEnumEx` to filter hardware H.264 encoders by the vendor ID of the adapter the game renders on. This is not a Skate executable RVA dependency, but it is part of replay-export behavior and must be tracked separately from the game fingerprint inventory. The original API result and COM activation references are preserved; filtered activations are released.

**Failure/compatibility behavior:** replay-export quality configuration is independently skipped when its prefix differs; frame pacing is required for `start()` to report success. The encoder setup reads fields from the encoder object, so a candidate configure function is only compatible if the encoder structure fields/enum semantics remain valid. The frame hook must not set frame_permit on unrelated encoders.


## 18. `fixed_stop_entitlement.h` — synthetic local entitlement future completion

Consumers: `Extension/Progression/fixed_stop_entitlement_provider.cpp`, `fixed_stop_native_result.cpp`, and `fast_travel_unlock.cpp`.

This is a replacement path for the native CheckEntitlements asynchronous request, not a general-purpose entitlement bypass. The provider only completes requests when every requested identifier is one of the recognized fixed bus-stop visibility/collected entitlements and the configured ownership callback confirms ownership. Mixed batches are forwarded to the native implementation because the newer reducer evaluates all rows rather than the older existential behavior.

| Symbol / field | ReSkate-side contract | Inputs / outputs / side effects | Priority / status |
|---|---|---|---|
| `check_entitlements` | Native CheckEntitlements entry shared with `entitlement_trace`. | Entry fingerprint gate for the replacement path. The provider's public adapter receives `future_out`, requested identifier array wrapper and result promise; exact native caller ABI/callsite that invokes the adapter is outside this header and must remain separately mapped. | P1 for offline fast-travel unlock; entry identity only |
| `group_enumeration` | Native group-member enumeration inside CheckEntitlements. | Exact instruction block reserves an eID list, reads group begin/end at +0x88/+0x90, skips null members, obtains eID key and calls member lookup. The provider independently reproduces this behavior. | P1, semantic block identity |
| `result_vector_init` | Constructs 16-byte native result vector. | ABI: `void*(NativeResultVector*)`. Initializes data pointer, metadata and state fields; data pointer at +0, metadata at +8, state byte +0xc. | P1, call ABI/layout |
| `array_allocate` | Native allocator for result vectors and nested pointer arrays. | ABI: `void*(owner, size, alignment, allocator)`. Allocation includes an 8-byte header: capacity u32, initialized count u32, followed by elements. Allocator associates the allocation with the owner for native destruction. | P1, ownership-critical ABI |
| `result_element_init` | Initializes one 32-byte entitlement result row. | ABI: `void*(NativeResultElement*)`. Initializes native string/array members and their empty sentinels. Row fields: successful-ID array +0, eID string +8, failed-ID array +0x10, owned byte +0x18. | P1, call ABI/layout |
| `assign_string` | Native string assignment used for success IDs and eIDs. | ABI: `void*(destination, bytes, u32 length)`. Must create native-owned string values; a CRT string or pointer into temporary C++ storage is not interchangeable. | P1, ownership-critical |
| `construct_error` | Constructs the native error value. | ABI: `void*(NativeError*, u32 code, const char* message)`. Success path constructs code 0 and empty message in a 40-byte native error object. | P1, call ABI/layout |
| `complete_promise` | Completes the caller's promise with error and result vector. | ABI: `void(promise, NativeError*, NativeResultVector*)`. Called only after every result row is fully initialized. It is the asynchronous completion point and may run continuations. | P1, callback/ownership boundary |
| `destroy_vector` | Destroys the native result vector and nested row contents. | ABI: `void(NativeResultVector*)`. Called after promise completion; also called by scope guard on all failure exits after vector initialization. | P1, destructor ABI |
| `native_text_release` | Destroys the temporary native error string. | ABI: `void(void*)`, called on the error object after promise completion. This must be the native string release operation, not a generic deallocator. | P1, ownership-critical |
| `assign_future` | Writes the completed/null future state to caller output. | ABI: `void*(future_out, nullptr)` in the success path, after promise completion and before result vector destruction. | P1, state transition |
| `eid_key` | Returns native eID key descriptor. | ABI: `const void*(int)`; provider calls with index 0 once a non-null group member exists. | P1, direct call |
| `find_member_eid` | Resolves a group member's eID string. | ABI: `const char*(member, key)`; null means no eID for that member. Non-null result must be a valid NUL-terminated string of at most 255 bytes. | P1, direct call |
| `client_game_manager` | Global pointer location for client game manager. | Dereferenced to manager; group pointer is manager +0x70. The global must be readable and non-null. | P1, global/data identity |
| `check_entitlements_prefix`, `*_prefix` | Fingerprint gates for every native helper. | The provider refuses preparation if any required helper prefix or the group enumeration block differs. These are build guards, not evidence that a newer implementation is incompatible. | P1, validation only |

**Group/eID data contract:** manager +0x70 is the group object. Group +0x88/+0x90 is a begin/end range of 0x10-byte member rows; member pointer is at row +8. At most 64 rows are accepted. Null members and members without eIDs are skipped. eIDs are copied into owned fixed-size buffers, deduplicated by exact string and sorted lexicographically before result construction. Empty groups or groups with no eIDs fall back to the native request.

**Request classification:** requested identifiers are a pointer array whose count is stored at data pointer -4 with the high bit masked off. Maximum request count is 512; each identifier is validated as a NUL-terminated string of at most 255 bytes. Recognized fixed-stop identifiers are mapped to the static catalog in `fast_travel_unlock.cpp`; an optional ownership predicate filters which IDs ReSkate claims. No recognized IDs, malformed data, unsupported mixed batches, re-entry, missing group/eIDs, and allocation failure all preserve the native path.

**Result semantics and cleanup:** for each sorted group eID, the provider creates a result row containing the same owned-success-ID list and that eID. It marks each row owned, sets result metadata to eID count and state byte to 1, constructs a success error object, completes the promise, releases the temporary error string, assigns the future output to null and destroys the result vector. Allocation headers' initialized counts are advanced after each successful initialization so native destruction can clean up partial construction. Scope guards preserve GetLastError while releasing native objects.

**Compatibility requirement:** the function candidates are not interchangeable solely by return value. The native future/promise state machine, allocator ownership, result vector/row layouts, native string representation, callback timing, and destructor behavior are all part of the observable contract. This adapter currently uses direct 20260929 function pointers; a newer build may need a rebuilt native result constructor or a different completion adapter even if entitlement semantics match.

## 19. `entitlement_trace.h` — original asynchronous entitlement request chain

Consumers: `Extension/Progression/fixed_stop_entitlement_provider.cpp` shares the entry/group contract; the trace header describes the unmodified native request chain and its callsite relationships.

| Symbol / site | ReSkate-side contract | Inputs / outputs / side effects | Priority / status |
|---|---|---|---|
| `check_entitlements` | Native CheckUsersEntitlements entry. | Same entry as fixed-stop provider. Receives the request context, creates promise/future state, enumerates the local group, builds the progression request and installs a continuation. | P1, shared entry identity |
| `request_builder` | Builds the asynchronous progression/entitlement request. | Called from the entry at `entry_builder_call`; receives an output/request object, manager/context and group-derived data. Exact C++ prototype and full request object layout are not declared in this header. | P1, call relationship; ABI incomplete |
| `result_reducer` | Converts service response rows into native entitlement result vector. | Called by continuation at `callback_reducer_call`; reducer computes successful rows and aggregate boolean. Its all-rows semantics are why mixed batches cannot safely be synthetically completed using the previous existential behavior. | P1, call relationship; ABI incomplete |
| `continuation_callback` | Async continuation invoked when request resolves. | Owns/captures continuation state and response object, then calls reducer. It can execute after the initiating CheckEntitlements frame has returned. | P1, callback/lifetime contract |
| `entry_promise_capture` | Entry instruction block captures/constructs promise state. | Stack-local promise/future-related objects and callback capture are initialized here. It is a callsite/constructor sequence, not a callable standalone function. | P1, instruction contract |
| `entry_builder_call` / `entry_builder_return` | Exact call to request builder and following return address. | The caller prepares request/output and group-derived input before calling builder. Return address is the control-flow join for the original async path. | P1, callsite identity |
| `callback_reducer_call` / `callback_reducer_return` | Exact reducer call from continuation. | Passes the response/result state to reducer; return address identifies the callback's post-reduction path. | P1, callsite identity |
| `group_enumeration` | Group member traversal. | Shared with fixed-stop provider. Group object member begin/end at +0x88/+0x90, row stride 0x10 and member pointer +8. | P1, data/call contract |

**Asynchronous lifetime:** the request builder's result is not the final entitlement result. The continuation is retained by the async operation and later invokes the reducer, which then completes the promise. Any behavioral replacement must preserve exactly-once completion, callback capture lifetime, promise/future ownership, and error propagation. Synchronous completion may be acceptable only if the caller's promise contract permits it; this requires testing rather than assuming.

**Known gap:** no consumer currently hooks or calls `request_builder`, `result_reducer` or `continuation_callback` directly. The fixed-stop adapter deliberately synthesizes a completed result instead of trying to replace the whole async chain. Exact prototypes, response object layout and error propagation of the original chain remain unresolved and should be recovered from disassembly/callers before attempting a general entitlement adapter.

## 20. `placement_adapters.h` and `placement_physics.h` — claimed object selection and moving physics surfaces

Consumers: `Extension/Objects/placement_adapters.cpp` and `placement_physics.cpp`.

### Selection and transform interception

| Symbol / site | ReSkate-side contract | Inputs / outputs / side effects | Priority / status |
|---|---|---|---|
| `select` | Native dropper selection start. | ABI: `void(controller, const entity_list*, bool quiet)`. Hook sets a thread-local selection controller and captures pose queries made during native selection, then forwards the original. Afterward it reconciles captured entities against the controller's selected-entity hash set and queues initial edit poses for claimed objects. | P1, hook ABI traced |
| `finish` | Native selection finish/commit/cancel. | ABI: `void(controller, const entity_list*, bool keep, bool cancel, bool force)`. Hook snapshots selected claimed entities when not canceling, forwards original, then detects which were actually deselected and queues final committed poses. | P1, hook ABI traced |
| `pose` | Retrieves transform for a selection query. | ABI: `float*(float out[12], const query*)`. Hook forwards original first. During selection, substitutes a cached/native rigid-body pose for a published claimed object unless it is already selected; records the pose so selection starts from the moving object's current transform. | P1, hook ABI traced |
| `transform_calls` | Exact transform-setter callsites inside dropper edit code. | All 11 are verified direct E8 calls. The hook computes caller return RVA and only records transform changes from these sites. The setter's ABI is `void(query, float pose[12], source)`; it forwards native behavior first. | P1, callsite set + hook ABI |
| `moving_calls` / `material_calls` | Native motion-type query callers in grind/surface code. | Each listed caller is a direct E8 call to `motion_type`. The hook returns keyframed for claimed dynamic surfaces at moving callers and fixed at material callers; other callers receive actual native motion type. | P1, exact caller discrimination |
| `friction` | Grind friction step. | Called with grind context and three floats; reads skater-context velocity as part of friction calculation. ReSkate wraps the call to preserve moving-surface behavior. | P1, hook role; exact scalar meanings unresolved |
| `point_velocity` | Rigid body velocity at a world point. | ABI: `float*(const Body*, float out[4], const float point[4])`; returns output vector. Used to derive support velocity for the skater on a claimed moving body. | P1, call ABI described |
| `shape_body` | Resolves physics body from shape handle. | ABI: `Body*(const Body* shape, Body* out)`. Output is a 16-byte FBPhysics handle. | P1, call ABI described |
| `edges` | Gathers authored grind edges around a grind context. | Hook gathers/rewrites edge geometry for claimed moving surfaces. Context has flags +0x13cc, position +0x70; disabled-query bit 0x400000 suppresses gathering. Query half extent is global vec3 (1.2,1.2,1.2). | P1, hook role and data fields |
| `edge_query` | Queries edges overlapping world bounds. | ABI: `bool(registry, const float bounds[8], out)`; bounds contain min xyz at floats 0..2 and max xyz at 4..6. Used to discover grindable geometry overlapping support bounds. | P1, call ABI described |
| `chain` | Resolves complete authored grind chain for edge identity. | ABI: `bool(const uint64 identity[2], NativeChain* out)`; output is edge pointer + u32 count. | P1, call ABI described |
| `root_flags` / `root_dynamic_restriction` | Entity root restriction flag. | Root record flags at +8; bit 0x800 makes placed object's body fixed. ReSkate clears the bit only in the returned root copy for a published claimed object, never in the entity's persistent root data. | P1, data/behavior identity |
| `selection_buckets`, `selection_count`, `selection_size`, `selection_next` | Native selection hash set. | Controller bucket pointer +0x248, bucket count +0x250, size +0x254, node next pointer +0x70; node begins with entity u64. Traversal bounded by validated counts. | P1, object layout |

### Geometry and lifetime

- Edge list data pointer is cache +0x10; at most 100 edges of 0x70 bytes are accepted. Shape-generation bytes are world-state +0xff8.
- Board support uses nine board parts, each with physics object at board +0x18 and parts at +0x20; owner backlink is part +0x10 and deck position +0x50. Seven downward support rays are cast from 0.4 above to 0.6 below the deck at lateral offsets 0, ±0.08, ±0.16, ±0.24.
- A chain is cached in local-body coordinates and transformed into two alternating world-space buffers. Double buffering prevents a contact worker from reading a buffer while the next pose overwrites it. Contacts can borrow these pointers across worker phases, so chain storage is retained for the life of that world and retired only when the next world is active. Total retained chain edges are bounded at 65,536.
- The cached edge/chain maps, support contact, surface context and published body map are world-scoped. World teardown marks them unavailable before any hook can dereference old-world pointers; stale chain storage is released only once the next world is active.

### Claimed-object physics service

| Symbol / field | ReSkate-side contract | Inputs / outputs / side effects | Priority / status |
|---|---|---|---|
| `body_pose` | Reads rigid body pose. | ABI: `float*(const Body*, float out[8])`; output is quaternion x/y/z/w then position x/y/z/pad. ReSkate expands it into a 12-float scale/rotation/position transform. | P1, call ABI |
| `sleep` | Sleeps/wakes a body. | ABI: `void(const Body*, bool asleep)`. Called to preserve native sleep behavior around changes. | P1, call ABI |
| `motion_type` | Reads body motion type. | ABI: `int(const Body*)`; hook changes what selected surface/grind callers observe, without changing the underlying body motion type. | P1, hook ABI |
| `marker_save` / `marker_return` | Local skater save/return marker hooks. | Save ABI: `void(controller, const float* transform, bool, bool, bool)`; return ABI: `void(controller, const float* transform, label, const void* options)`. ReSkate emits plugin marker events while forwarding native operation. | P2, hook ABI |
| `marker_owner`, `marker_transition`, `marker_transform` | Marker controller fields. | Owner handle +0x18 begins with skater entity; transition byte/state +0x40 is 0 before return and 1 once return starts; saved transform at +0x50. | P2, data layout |
| `world_state`, `velocity_buffer`, `velocity_count`, `velocity_page`, `mass_buffer` | Physics world per-body arrays. | World +0x28 -> world state; velocity buffer +0x930, count +0x938, page +0x93c, inverse-mass buffer +0x980. Page stride is align64(count*16+60). | P1, layout identity |
| `store_ready`, `records_begin`, `records_end`, `record_size` | Client realm placement store. | Store ready byte +0x100; record begin/end +0x48/+0x50; record size 0x50. Each record active byte +0x3a, drop ID u32 +0x28 and entity u64 +0x10. | P1, layout identity |

**Body handle ABI:** every FBPhysics handle is 16 bytes: world pointer, u32 body index and u32 generation. Generation validation is essential; a recycled index is not the same body. Inverse mass is read from the world-state mass buffer and validated finite/positive.

**Client/server ownership:** plugin API and client tick update only the client replica's transform/body. Server tick, on the thread owning the server copy, mirrors moving poses and performs queued restores. Both coordinate ReSkate state with `native_mutex`; they must not write the other thread's native copy. Each claim stores owner, token, map generation, server/client entities, body handle, fixed layout pose, options, edit revision and server motion type. If server mirroring cannot resolve or the game moves the server copy independently, the client claim is released rather than allowing divergence.

**Claim/release semantics:** a lease preserves the original park-layout pose. On release, the client body returns to that pose and a restore is queued for the server copy. Restores are applied only if row identity, token and map generation still match. World teardown drops leases without writing into destroyed bodies. These conditions are behaviorally essential; a matching body-pose or motion-type function alone cannot reproduce the service.

**ABI gaps:** `edges`, `friction`, selection entity-list representation, transform setter source enum and marker label/options layouts are not fully reconstructed here. The header's 16-byte function prefixes and exact callsite lists are build guards. Recovered semantics do not prove a candidate has the same ABI.

## 21. `mission_progression.h` — quest lookup and ECS status override

Consumer: `Extension/Progression/mission_progression_override.cpp`; observation only in `mission_progression_observation.cpp`.

| Symbol / site | ReSkate-side contract | Inputs / outputs / side effects | Priority / status |
|---|---|---|---|
| `quest_lookup` | Resolves QuestId wrapper through native quest manager. | Hook preserves native result and GetLastError, reads the wrapper's first pointer as NUL-terminated identifier (maximum 63 chars, restricted to alphanumeric/underscore), and tags the returned 64-bit handle only for allowlisted quest IDs. Handle high 32 bits must be nonzero. | P1 for offline progression; hook ABI should be checked against native caller |
| `component_query` | Generic ECS component query. | Hook preserves native call and return value. Only the exact quest-status query return address is eligible for state mutation; all other component queries are passed through untouched. | P1, hook ABI traced |
| `quest_status_query_call` / `quest_status_query_return` | Exact native quest status query callsite and post-call consumer. | Native call initializes a 24-byte query result, supplies the quest status component descriptor, and invokes component_query. The post-call code reads the component's state dword at +0x18. The two RVAs are adjacent around the E8 call and are checked as a paired callsite contract. | P1, instruction/callsite identity |
| `quest_status_component_descriptor` | Descriptor for the QuestStatus ECS component. | Passed to generic component query; pointer identity is required before ReSkate considers changing the returned component. | P1, data identity |
| `quest_expression_call` / `quest_expression_return` | Compiled mission expression QuestId lookup callsite. | Exact return address distinguishes expression lookups from unrelated quest lookups. Used to hydrate persisted profile state into a freshly resolved component identity. | P1, callsite identity |
| `query_context_constructor` / `query_context_initializer` | Native ECS query context construction. | Constructor builds a temporary three-word context; initializer is fingerprinted but consumer does not call it directly in the shown hydration path. Constructor receives context storage and handle. | P1, direct call + related identity |
| `query_result_vtable` / `quest_status_type` | Native query result object identity and expected TypeInfo. | ReSkate constructs a five-qword result prefix with vtable, handle and type pointer, calls the original component query, then reads result guard and component pointers. | P1, object layout/identity |

**Quest component state:** the component's signed 32-bit state is at +0x18; valid range is 0..6 and claimed/completed state is 5. Temporary offline override leases save the original state and only restore it if the same component pointer and ECS handle still identify the same entity and current state is still 5. If native gameplay has changed it, restoration is skipped. This avoids overwriting a legitimate state transition.

**Persisted profile hydration:** profile lookup and completion-save callbacks provide durable state. Only explicit persisted values 0 or 5 are applied. The code resolves a fresh ECS query context and verifies entity identity/generation before writing. A saved reset to 0 is applied once to a freshly resolved identity and is not pinned while the player attempts the mission again. These durable profile writes are separate from temporary debug leases.

**Hook and thread behavior:** quest lookup tags are thread-local and generation-scoped; component query consumes and clears the tag so it cannot leak to a later unrelated query. Both hooks are prepared together and enabled in sequence. If enabling the second fails, both are disabled while created hooks/trampolines are retained because an in-flight thread may already have entered a detour. Gate states distinguish image/fingerprint mismatch, hook creation/enable failure, active, restoring and route-required. GetLastError is preserved around the adapters.

**ABI gaps:** exact quest manager/lookup native prototype, ECS context internals beyond the three-word prefix, result guard meaning and full QuestStatus component schema are not fully reconstructed. Candidate matching must preserve caller filtering, query identity and conditional restore semantics, not just the state offset.

## 22. `local_location_travel.h` and `location_travel.h` — offline destination catalogs and local travel dispatch

Consumers: `Extension/Travel/local_location_travel.cpp`, `Engine/Game/World/location_travel.h`, and profile runtime initialization.

| Symbol / site | ReSkate-side contract | Inputs / outputs / side effects | Priority / status |
|---|---|---|---|
| `owner_vtable` | Native LocationTravel owner identity. | Global owner is `engine::location_owner`; owner vtable must match this identity. Location list pointer at owner +0x20, access-point list at +0x40. | P1 for local travel UI |
| `location_type` / `access_point_type` | Native model TypeInfos. | Used to find/create data-model contexts for destination and access-point records. | P1, TypeInfo identity |
| `notify_event_types` | Two ECS events dispatched when catalogs are ready. | Dispatches empty event payload with options {0,1,0} through challenge runtime's event dispatcher and current challenge owner context. Sent once after publication, outside the model write lock. | P1, event identity |
| `access_point_record`, `location_record`, `byte_records` | TypeInfo record layouts. | Validates hashes and sizes: access point 0x28, location 0xc0, two byte-sized records. Rejects the catalog if the target build's type record hash/size differs. | P1, data layout identity |
| `travel_location_ctor_contract` / `travel_access_ctor_contract` | Native constructors for destination and access-point records. | Fingerprints for native constructors invoked through runtime-resolved callbacks. Consumer's TravelValue buffers are 0xc0 and 0x28 bytes respectively. Exact C++ constructor prototypes are provided by runtime initialization, not these headers. | P1, constructor ABI; exact prototypes unresolved here |
| `travel_prepare_contract` | Native preparation of travel request output. | Called by the runtime-resolved `prepare_request(output, identifier)` before local overrides; returns native output pointer. ReSkate then assigns destination identifier into two native CString slots at output +0 and +8. | P1, call ABI through runtime pointer |
| `travel_request_contract` | Scenario travel request entry. | Hook receives two opaque arguments; if the first is a local destination it queues a local map transition and suppresses the online/native request. Otherwise it forwards through the runtime callback. | P1, hook behavior; exact native ABI needs caller tracing |
| `travel_attribute_request_contract` | Attribute-only travel request entry. | Hook receives attributes and options; local destination queues map transition and suppresses matchmaking request. Non-local IDs forward to native handler. | P1, hook behavior; exact native ABI needs caller tracing |

**Catalog source and filtering:** the policy is built from cached native travel-location and access-point catalogs. Only destinations whose level tags map to supported local maps (BAM, Isle of Grom, MPR, Stadium) are retained; each destination needs a name and the policy caps destinations at 16. Access points retain only unique target IDs that exist in the filtered destination catalog. Artwork links using the game's `cdn:/` scheme are converted to the Windows CDN rendition URL.

**Native model publication:** once per second, ReSkate validates the active offline runtime, owner vtable, model manager, owner list handles, array TypeInfo and all four record hash/size pairs. It takes the model write lock and only publishes when both native lists are empty or still match ReSkate's previously published contexts. It creates/finds destination contexts, constructs native 0xc0-byte location values and fills fields including IDs, labels, description, icons, image URLs, medium, name hash and unlocked byte. It similarly creates 0x28-byte access-point values with target destination hashes. It publishes context arrays to the two native list handles, then sets owner ready bytes +0x180 and +0x182. This is not a generic UI list: the native data model owns the records and list references.

**Notification and lifetime:** catalog-ready events are dispatched after leaving the model write lock. Contexts and borrowed string values are passed through native model publication, which copies borrowed values. ReSkate retains its expected context references to detect replacement by another provider; it resets readiness if owner/model/list identity changes. The notification is retried on later ticks until the challenge owner context exists.

**Local travel dispatch:** the prepare hook replaces both native output CString slots with the local destination ID so both menu submission branches resolve locally. The two request hooks detect local IDs (including two exact VM layouts for scenario requests), enqueue the target map through an atomic callback, and suppress native matchmaking requests. A queue failure is logged and the request remains local rather than falling through to matchmaking. Non-local destinations retain the native path.

**Unknowns:** full native TravelLocation/AccessPoint field schemas beyond the offsets written here, exact constructors' argument prototypes, travel request callsite arguments and event consumer semantics remain unresolved. The local queue callback's implementation and level transition contract are tracked under level loading/runtime, not duplicated here.


## 23. Full 20260929 header inventory audit (initial pass)

Audit basis: recursive Git tree of `d3vn10/ReSkatePlus` on `main`, compared with this document's contract sections. The tree contains exactly **137** headers in `Engine/Game/Build/20260929/`. The 24 headers already expanded are listed below. The remaining 113 are inventoried in the table with a first-pass priority and any source files whose paths share the header stem.

**Status definitions**
- **Expanded:** a dedicated contract section exists above. This means the declaration surface and important consumer behavior have been documented, not that every ABI/layout is solved.
- **Source located:** a same-stem source/header exists in the tree. This is only a lead; actual include/call-site usage still needs verification.
- **No same-stem source found:** not evidence of dead code. It may be included by a differently named consumer, used through a shared address namespace, or be declaration-only. Search include references and call sites before classifying it as unused.

**Expanded (24):** `engine.h`, `runtime.h`, `level_loading.h`, `native_party.h`, `profile.h`, `no_bail.h`, `park_editor.h`, `native_throwdowns.h`, `loose_files.h`, `live_mods.h`, `mod_layers.h`, `world_layers.h`, `streaming_reads.h`, `native_skater.h`, `native_indicators.h`, `native_player_ui.h`, `replay_export.h`, `fixed_stop_entitlement.h`, `entitlement_trace.h`, `placement_adapters.h`, `placement_physics.h`, `mission_progression.h`, `local_location_travel.h`, `location_travel.h`.

**Priority meaning:** P0 = startup, core runtime, world/asset boot, core skater or physics correctness; P1 = major gameplay, multiplayer, progression, rendering/resource, or world behavior; P2 = optional presentation, diagnostics, peripheral systems. This is an investigation order, not a claim that every symbol in a header is equally important.

| Header | Priority | Same-stem consumer leads (not yet verified as complete call graph) |
|---|---:|---|
| `ai_skaters.h` | P2 / unranked | `Extension/Skater/ai_skaters.cpp`, `Extension/Skater/ai_skaters.h` |
| `animation_values.h` | P2 / unranked | `Engine/Game/Skater/animation_values.h` |
| `atmosphere.h` | P1 | `Engine/Game/World/atmosphere_controls.h`, `Extension/UI/Overlay/atmosphere_color.h` |
| `audio_core.h` | P2 | No same-stem source found; include/call graph still needs inspection |
| `body_contact.h` | P1 | `Engine/Game/World/Test/body_contact_read_tests.cpp`, `Engine/Game/World/body_contact_read.h` |
| `buildkit_limits.h` | P2 | `Extension/Objects/local_buildkit_limits.cpp`, `Extension/Objects/local_buildkit_limits.h` |
| `camera_observer.h` | P2 / unranked | `Extension/Skater/Test/camera_observer_tests.cpp`, `Extension/Skater/camera_observer.cpp` |
| `client_source_spawn.h` | P2 / unranked | `Extension/Skater/client_source_spawn.cpp`, `Extension/Skater/client_source_spawn.h` |
| `debug_settings_access.h` | P2 | No same-stem source found; include/call graph still needs inspection |
| `decals.h` | P2 | `Extension/Decals/native_decals.h` |
| `descriptor_heap_size.h` | P2 / unranked | `Extension/Boot/descriptor_heap_size.cpp`, `Extension/Boot/descriptor_heap_size.h` |
| `eadp_errors_arena.h` | P2 / unranked | `Extension/Boot/eadp_errors_arena.cpp`, `Extension/Boot/eadp_errors_arena.h` |
| `effect_surface.h` | P2 | `Extension/Rendering/effect_surface.h` |
| `effects.h` | P2 | `Extension/Multiplayer/Net/effects.h`, `Extension/Plugins/plugin_api_effects.cpp` |
| `engine_tweaks.h` | P2 / unranked | `Extension/Settings/engine_tweaks.cpp`, `Extension/Settings/engine_tweaks.h` |
| `entity_pages.h` | P1 | `Extension/World/native_entity_pages.cpp`, `Extension/World/native_entity_pages.h` |
| `frame_generation.h` | P2 | `Extension/Rendering/frame_generation_retain.cpp`, `Extension/Rendering/frame_generation_retain.h` |
| `game_data_files.h` | P0 | No same-stem source found; include/call graph still needs inspection |
| `game_fatal_dump.h` | P2 / unranked | `Extension/Boot/game_fatal_dump.cpp`, `Extension/Boot/game_fatal_dump.h` |
| `game_state_values.h` | P0 | No same-stem source found; include/call graph still needs inspection |
| `game_states.h` | P0 | No same-stem source found; include/call graph still needs inspection |
| `gameplay_settings.h` | P2 / unranked | `Extension/Settings/gameplay_settings_internal.h`, `Extension/Settings/gameplay_settings_leases.cpp` |
| `global_offline.h` | P0 | No same-stem source found; include/call graph still needs inspection |
| `graph_throttle.h` | P2 / unranked | `Extension/Throwdowns/graph_throttle.cpp`, `Extension/Throwdowns/graph_throttle.h` |
| `graphics_controls.h` | P2 / unranked | `Engine/Game/Rendering/graphics_controls.h`, `Extension/Rendering/local_graphics_controls.cpp` |
| `graphics_labels.h` | P2 | `Extension/Rendering/graphics_labels.cpp`, `Extension/Rendering/graphics_labels.h` |
| `grind_target.h` | P1 | `Extension/Skater/grind_target.cpp`, `Extension/Skater/grind_target.h` |
| `guid_table_lookup.h` | P1 | No same-stem source found; include/call graph still needs inspection |
| `hud_corner.h` | P2 | `Extension/UI/hud_corner.cpp`, `Extension/UI/hud_corner.h` |
| `initfs.h` | P0 | `Engine/Vfs/initfs.cpp`, `Engine/Vfs/initfs.h` |
| `job_system.h` | P0 | No same-stem source found; include/call graph still needs inspection |
| `launches.h` | P1 | `Extension/Skater/launches.cpp`, `Extension/Skater/launches.h` |
| `light_tables.h` | P1 | No same-stem source found; include/call graph still needs inspection |
| `load_errors.h` | P2 / unranked | No same-stem source found; include/call graph still needs inspection |
| `loading_screen.h` | P2 / unranked | `Engine/Game/UI/loading_screen_selection.h`, `Extension/World/Test/loading_screen_tests.cpp` |
| `local_challenge.h` | P1 | `Extension/Progression/local_challenge_runtime.cpp`, `Extension/Progression/local_challenge_runtime.h` |
| `local_cosmetic_catalog.h` | P1 | `Extension/Customization/local_cosmetic_catalog.cpp`, `Extension/Customization/local_cosmetic_catalog.h` |
| `local_customization.h` | P1 | `Extension/Customization/local_customization.cpp`, `Extension/Customization/local_customization.h` |
| `local_music.h` | P2 / unranked | `Extension/Music/Test/local_music_favorites_tests.cpp`, `Extension/Music/Test/local_music_playback_policy_tests.cpp` |
| `local_native_settings.h` | P2 / unranked | `Extension/Settings/local_native_settings.cpp`, `Extension/Settings/local_native_settings.h` |
| `local_neighborhood.h` | P1 | `Extension/Progression/local_neighborhood_runtime.cpp`, `Extension/Progression/local_neighborhood_runtime.h` |
| `local_object.h` | P1 | `Extension/Objects/local_object_runtime.cpp`, `Extension/Objects/local_object_runtime.h` |
| `local_placements.h` | P1 | `Extension/Objects/local_placements_runtime.cpp`, `Extension/Objects/local_placements_runtime.h` |
| `local_player_card.h` | P2 / unranked | `Extension/Customization/local_player_card_runtime.cpp`, `Extension/Customization/local_player_card_runtime.h` |
| `local_rip_score.h` | P2 / unranked | `Extension/Progression/local_rip_score_runtime.cpp`, `Extension/Progression/local_rip_score_runtime.h` |
| `localization.h` | P1 | `Extension/Localization/text_overrides.cpp`, `Extension/Localization/text_overrides.h` |
| `lua_natives.h` | P0 | No same-stem source found; include/call graph still needs inspection |
| `lua_startup.h` | P0 | `Extension/Scripting/lua_startup.cpp`, `Extension/Scripting/lua_startup.h` |
| `media_catalog.h` | P1 | No same-stem source found; include/call graph still needs inspection |
| `mesh_settings.h` | P1 | `Extension/World/mesh_settings_guard.cpp`, `Extension/World/mesh_settings_guard.h` |
| `mesh_variations.h` | P1 | No same-stem source found; include/call graph still needs inspection |
| `morph_memory_pool.h` | P2 / unranked | `Extension/Assets/morph_memory_pool.cpp`, `Extension/Assets/morph_memory_pool.h` |
| `named_settings.h` | P2 / unranked | `Engine/Game/Settings/named_settings.h`, `Extension/Settings/named_settings.cpp` |
| `native_audio.h` | P1 | `Extension/Multiplayer/Remote/native_audio.cpp`, `Extension/Multiplayer/Remote/native_audio.h` |
| `native_cosmetics.h` | P1 | `Extension/Multiplayer/Remote/native_cosmetics.cpp`, `Extension/Multiplayer/Remote/native_cosmetics.h` |
| `native_menu.h` | P1 | `Extension/UI/NativeMenu/Test/native_menu_lifetime_tests.cpp`, `Extension/UI/NativeMenu/Test/native_menu_view_tests.cpp` |
| `native_patch_support.h` | P0 | `Extension/Assets/native_patch_support.cpp`, `Extension/Assets/native_patch_support.h` |
| `native_vfx.h` | P1 | `Extension/Multiplayer/Remote/native_vfx.cpp`, `Extension/Multiplayer/Remote/native_vfx.h` |
| `native_voice.h` | P1 | `Extension/Multiplayer/Voice/native_voice.cpp`, `Extension/Multiplayer/Voice/native_voice.h` |
| `neighborhood_unlock.h` | P1 | `Extension/Progression/neighborhood_unlock_override.cpp`, `Extension/Progression/neighborhood_unlock_override.h` |
| `offboard_flight.h` | P2 / unranked | `Extension/Skater/offboard_flight.cpp`, `Extension/Skater/offboard_flight.h` |
| `park_rotation.h` | P2 | `Engine/Game/World/park_rotation.h`, `Extension/World/local_park_rotation.cpp` |
| `pending_updates.h` | P1 | No same-stem source found; include/call graph still needs inspection |
| `physics_bodies.h` | P0 | No same-stem source found; include/call graph still needs inspection |
| `physics_tuning.h` | P1 | `Extension/Skater/Test/physics_tuning_tests.cpp`, `Extension/Skater/physics_tuning.cpp` |
| `physics_wake.h` | P1 | `Extension/World/physics_wake_guard.cpp`, `Extension/World/physics_wake_guard.h` |
| `physics_world_size.h` | P1 | `Extension/World/physics_world_size.cpp`, `Extension/World/physics_world_size.h` |
| `population.h` | P1 | `Extension/World/local_population_controls.cpp`, `Extension/World/local_population_controls.h` |
| `profiler_labels.h` | P2 | No same-stem source found; include/call graph still needs inspection |
| `progression_service_guard.h` | P2 / unranked | `Extension/Progression/progression_service_guard.cpp`, `Extension/Progression/progression_service_guard.h` |
| `protossl.h` | P2 | No same-stem source found; include/call graph still needs inspection |
| `puppet_cost.h` | P2 | `Extension/Multiplayer/Remote/puppet_cost.cpp`, `Extension/Multiplayer/Remote/puppet_cost.h` |
| `ragdoll_bodies.h` | P2 | No same-stem source found; include/call graph still needs inspection |
| `reference_lists.h` | P1 | No same-stem source found; include/call graph still needs inspection |
| `remote_collision.h` | P0 | `Extension/Multiplayer/Remote/remote_collision.cpp`, `Extension/Multiplayer/Remote/remote_collision.h` |
| `render_resource_pool.h` | P2 / unranked | `Extension/Assets/native_render_resource_pool.cpp`, `Extension/Assets/native_render_resource_pool.h` |
| `replay_activity.h` | P2 | No same-stem source found; include/call graph still needs inspection |
| `replay_keyframes.h` | P1 | `Extension/ReplayKeyframes/Test/replay_keyframes_tests.cpp`, `Extension/ReplayKeyframes/replay_keyframes.cpp` |
| `replay_recorder.h` | P1 | No same-stem source found; include/call graph still needs inspection |
| `rigid_bodies.h` | P0 | No same-stem source found; include/call graph still needs inspection |
| `route_lookahead.h` | P2 | `Extension/World/native_route_lookahead.cpp`, `Extension/World/native_route_lookahead.h` |
| `score_hud.h` | P2 | `Extension/Skater/score_hud.cpp`, `Extension/Skater/score_hud.h` |
| `script_natives.h` | P0 | `Extension/Progression/script_natives.h` |
| `shader_permutations.h` | P1 | No same-stem source found; include/call graph still needs inspection |
| `shader_registry.h` | P1 | `Extension/World/shader_registry_guard.cpp`, `Extension/World/shader_registry_guard.h` |
| `skater_body.h` | P1 | `Engine/Game/Skater/skater_body.h` |
| `skater_entities.h` | P0 | No same-stem source found; include/call graph still needs inspection |
| `skater_lead_foot.h` | P2 / unranked | No same-stem source found; include/call graph still needs inspection |
| `skater_model.h` | P1 | `Extension/Skater/skater_model.cpp` |
| `skater_movement.h` | P1 | `Extension/Plugins/plugin_skater_movement.h`, `Extension/Skater/skater_movement.cpp` |
| `skater_observer.h` | P1 | `Extension/Skater/skater_observer.cpp`, `Extension/Skater/skater_observer.h` |
| `skater_render.h` | P1 | No same-stem source found; include/call graph still needs inspection |
| `skater_skeleton.h` | P1 | `Extension/Skater/skater_skeleton.h` |
| `skater_slot_override.h` | P2 / unranked | `Extension/Skater/skater_slot_override.cpp`, `Extension/Skater/skater_slot_override.h` |
| `skater_state.h` | P0 | No same-stem source found; include/call graph still needs inspection |
| `spatial_buckets.h` | P2 | No same-stem source found; include/call graph still needs inspection |
| `spatial_cull_ids.h` | P2 | `Extension/World/spatial_cull_ids.cpp`, `Extension/World/spatial_cull_ids.h` |
| `startup_interventions.h` | P0 | `Extension/Boot/startup_interventions.cpp`, `Extension/Boot/startup_interventions.h` |
| `texture_handles.h` | P1 | No same-stem source found; include/call graph still needs inspection |
| `texture_streaming.h` | P1 | No same-stem source found; include/call graph still needs inspection |
| `throwdown_debug_text.h` | P2 | `Extension/Throwdowns/throwdown_debug_text.cpp`, `Extension/Throwdowns/throwdown_debug_text.h` |
| `traffic_vehicles.h` | P1 | `Extension/World/traffic_vehicles.cpp`, `Extension/World/traffic_vehicles.h` |
| `trick_rewrite.h` | P2 / unranked | `Extension/Skater/trick_rewrite.cpp`, `Extension/Skater/trick_rewrite.h` |
| `ui_model.h` | P1 | No same-stem source found; include/call graph still needs inspection |
| `ui_pointer.h` | P2 / unranked | `Extension/UI/ui_pointer_skip.cpp`, `Extension/UI/ui_pointer_skip.h` |
| `ui_textures.h` | P1 | No same-stem source found; include/call graph still needs inspection |
| `ui_values.h` | P2 / unranked | `Extension/Plugins/plugin_ui_values.cpp`, `Extension/Plugins/plugin_ui_values.h` |
| `unload_guard.h` | P2 / unranked | `Extension/World/mesh_unload_guard.cpp`, `Extension/World/mesh_unload_guard.h` |
| `visual_environment.h` | P1 | `Extension/World/visual_environment.cpp`, `Extension/World/visual_environment.h` |
| `world_controls.h` | P0 | `Engine/Game/World/world_controls.h`, `Extension/World/local_world_controls.cpp` |
| `world_model.h` | P0 | `Engine/Game/World/world_model.cpp`, `Engine/Game/World/world_model.h` |
| `world_pool_usage.h` | P2 | `Extension/World/world_pool_usage.cpp`, `Extension/World/world_pool_usage.h` |
| `world_props.h` | P2 / unranked | No same-stem source found; include/call graph still needs inspection |

### Audit conclusions and next investigation order

1. **Boot and asset foundations (P0):** `initfs.h`, `global_offline.h`, `game_states.h`, `game_state_values.h`, `startup_interventions.h`, `job_system.h`, `game_data_files.h`, `native_patch_support.h`, `guid_table_lookup.h`, `media_catalog.h`, `reference_lists.h`, `entity_pages.h`, `pending_updates.h`. These may determine whether the game reaches a usable world and whether mod assets resolve. `initfs` has `Engine/Vfs/initfs.cpp`; several others have no same-stem source and need include/call-site discovery.
2. **World and skater core (P0/P1):** `world_model.h`, `world_controls.h`, `skater_entities.h`, `skater_state.h`, `skater_body.h`, `skater_movement.h`, `skater_model.h`, `skater_skeleton.h`, `skater_observer.h`, `skater_render.h`, `launches.h`, `grind_target.h`, `physics_bodies.h`, `rigid_bodies.h`, `remote_collision.h`, `physics_tuning.h`, `physics_wake.h`, `physics_world_size.h`, `body_contact.h`.
3. **Multiplayer runtime (P1):** `native_audio.h`, `native_cosmetics.h`, `native_vfx.h`, `native_voice.h`, plus `remote_collision.h` and puppet/skater support. The same-stem consumer paths exist for all four native multiplayer headers, so these should be traced before low-impact UI and diagnostic headers.
4. **Progression, local services and customization (P1):** `local_neighborhood.h`, `local_challenge.h`, `local_cosmetic_catalog.h`, `local_customization.h`, `local_object.h`, `local_placements.h`, `neighborhood_unlock.h`, `local_player_card.h`, `local_music.h`, `local_rip_score.h`.
5. **UI, replay and presentation (P1/P2):** `native_menu.h`, `ui_model.h`, `ui_pointer.h`, `ui_textures.h`, `ui_values.h`, `localization.h`, `replay_recorder.h`, `replay_keyframes.h`, `replay_activity.h`, `hud_corner.h`, `score_hud.h`, `graphics_controls.h`, `graphics_labels.h`, `frame_generation.h`.
6. **Remaining lower-priority systems:** diagnostics, profiler labels, throwdown debug text, audio core, decals/effects, route lookahead, park rotation, buildkit limits, ProtoSSL and remaining specialized resource/physics helpers.

### Coverage caveats

- This is a **header inventory audit**, not yet a complete declaration-level coverage audit. The next pass must count each header's functions, globals, offsets, callsite constants and byte-prefix guards, then compare those identifiers with documented rows.
- Same-stem source matching is intentionally conservative and may miss consumers whose filenames differ. Conversely, a matching source filename does not prove it includes or calls the build-specific header.
- Headers that have no dedicated section may still have been mentioned in earlier high-level notes in the project history. They remain unexpanded here until their actual consumer and contract are traced.
- The `profile.h` section is broad and high-density; it needs a separate completeness audit against all declarations and the actual profile consumers.
- The existing sections contain explicit ABI gaps. Those remain unresolved and must not be upgraded to confirmed candidate matches solely because a fingerprint or structural match exists.


## 24. `initfs.h` — encrypted InitFS archive key extraction

Consumer: `Engine/Vfs/initfs.cpp`.

| Symbol / data | Contract | Compatibility significance |
|---|---|---|
| `key` | RVA of a 16-byte AES key embedded in the executable image. | Data identity, not a callable function. Key bytes are extracted from the on-disk executable only after the launcher validates the full executable identity. |
| `decrypt_reference` / `decrypt_reference_prefix` | RVA and 18-byte instruction prefix of the decryptor's reference to the key. | The reference bytes are checked before reading the key. A moved key with a stale reference or a matching reference with changed key semantics must not be accepted. |

**Consumer behavior:** the loader parses the executable PE section table, maps the key RVA into a file-backed section, verifies the 18-byte decrypt-reference sequence, and reads exactly 16 key bytes. AES-CBC decryption uses that key as both AES key and initial IV, with PKCS padding. Secret buffers and AES key material are zeroed on destruction. Archive input is capped at 64 MiB, validated as a native DB envelope, parsed with full-consumption checks, and merged into an effective entry map.

**Compatibility contract:** the key is build-specific data and the decrypt-reference prefix is its locator guard. There is no function ABI to relocate. A new executable requires independently recovering the key and verifying the encryption mode/IV/padding and native DB envelope. Do not infer compatibility from the key RVA alone.

## 25. `global_offline.h` — shared OnlineEnabled leaf and caller-specific offline behavior

Consumer: `Extension/Boot/offline_boot.cpp`.

| Symbol / site | Contract | Compatibility significance |
|---|---|---|
| `online_enabled` | Shared leaf, ABI `uint8_t()`, currently returns true natively. | Hook target and trampoline. The body is only `MOV AL,1; RET`; the 16-byte prefix includes inspected INT3 padding needed by the hook service. |
| `backend_predicate_return` | Exact caller return address for backend-online predicate. | This caller receives the offline-specific false result. |
| `async_prepare_character_return` | Exact caller return address for async character preparation. | Caller-specific behavior is applied only at this native decision point. |
| `character_readiness_return` | Exact caller return address for character readiness. | Separately classified so offline boot does not globally falsify every OnlineEnabled query. |
| `flow_adapter_return` | Return address in reflected Flow getIsOnlineEnabled adapter. | Tail-JMP thunk means the leaf observes the adapter return address rather than the apparent outer callsite. |
| `post_spawn_progression_return` | Post-spawn progression guard caller. | Offline-specific result suppresses a progression path that expects online backend state. |

**Activation contract:** opt-in is explicit through `RESKATE_GLOBAL_OFFLINE=1` or the legacy debug variable. The consumer verifies the supported image, checks the leaf prefix, probes the backend singleton before enabling the hook, and refuses activation if boot has progressed too far. Hook states distinguish opt-out, image/fingerprint mismatch, backend probe failure, late activation, create/enable/disable failures, activation in progress and active. The hook preserves GetLastError.

**Caller filtering:** the detour classifies the exact return address and only substitutes results for the five enumerated call paths. Unmatched callers are forwarded to the original. The distinction is essential: globally returning false would alter unrelated engine online checks and may break offline UI or local profile initialization.

**Compatibility requirement:** preserve both the tiny leaf ABI and its caller-specific semantics. A replacement candidate must be paired with freshly identified callsites and thunk behavior. Matching the three-byte body alone is insufficient; the current 16-byte prefix includes padding used by the detour implementation.

## 26. `world_model.h` — level registry and server sublevel manager globals

Consumer: `Engine/Game/World/world_model.cpp`; the world catalog is used by map/level discovery.

| Symbol / data | Contract | Compatibility significance |
|---|---|---|
| `level_registry` | Image global containing pointer to level registry. Registry begin/end pointers are at +0x20/+0x28. | Global pointer and container layout; not a function. |
| `sublevel_manager` | Image global containing pointer to server sublevel manager. | Global pointer and manager/tree layout; not a function. |

**Level registry layout:** the registry points to begin/end pointers at +0x20/+0x28. The range is an array of 8-byte metadata pointers, bounded to 128 entries. Each metadata object has a start-point array pointer at +0x20, asset-name pointer at +0x48 and level flag at +0x68. Start-point rows are 0x18 bytes; name pointer +8 and default flag +0x10. Start-point count is read from the array header at data pointer -4, high bit masked, and capped at 64. Asset names must be safe `levels/...` paths without `..`.

**Sublevel manager layout:** manager +0x40 is current root, +0x58 is tree root, +0xb0 is active byte; +0x48 is the sentinel. Tree nodes have left/right links at +0/+8, state at +0x28 and asset-name pointer at +0x30. Traversal rejects cycles, duplicate nodes and more than 128 nodes. The manager header is reread after traversal and must remain unchanged, preventing a torn catalog while the tree mutates.

**Consumer behavior:** reads are performed against a target process handle with ReadProcessMemory and strict pointer/range/alignment/count checks. The resulting catalog is descriptive only; it does not own native level objects or retain pointers after the read. Failure returns an unavailable catalog with a diagnostic reason rather than partially trusting malformed metadata.

**Compatibility requirement:** the two RVAs, registry offsets, metadata/row strides, string pointer offsets, and sublevel tree layout must be validated as one contract. A match on either global alone is insufficient.

## 27. `launches.h` — launch trajectory interception and grind-pop classification

Consumer: `Extension/Skater/launches.cpp`; related measurements and trainer integration in `Extension/Trainer/trainer_jump.cpp`.

| Symbol / site | Contract | Compatibility significance |
|---|---|---|
| `set_trajectory` | Native jump-trajectory setter, declared `void(trajectory, parameters)`; 32-byte prefix guard. | Hook target. The consumer's detour modifies the three-float launch velocity in the parameter object and forwards native behavior. |
| `grind_pop_call`, `ollie_call`, `boneless_call` | Exact direct callsites of the trajectory setter. | Caller return RVA classifies launch origin: grind pop/no-comply, flat-ground ollie, boneless. Grind pop and no-comply share a callsite and are disambiguated by recent physics state. |
| `grind_states_first/end`, `other_grind_state`, `pop_state` | Physics-state classification: grind states [400,500), plus 701; pop state 200. | Used with a fresh physics-step timestamp to decide whether the shared callsite is a grind pop or no-comply. |
| `query_manager` / `query_manager_load` | Global trajectory query manager pointer and its load instruction. | Related landing/trajectory query system and gravity source; not directly hooked by this consumer. |
| `stance_flags_offset`, `stance_switch_mask` | Skater context stance flags at +0x13c4, switch bit 0x20. | Used to expose stance state alongside launch measurements. |
| `foot_gravity` / `foot_gravity_prefix`, `foot_speed_cap` | On-foot gravity instruction and walking speed cap. | Trainer-facing build data; not the main launches detour. Retain separately from launch setter compatibility. |

**Physics-thread data flow:** each local skater physics step reads the current physics state, stance flag and board root-body velocity. Horizontal board velocity is averaged over six recent steps and published atomically with a QPC timestamp. The launch hook classifies the caller, checks whether the physics state is a fresh grind state, snapshots the plugin-selected launch shape under a mutex, then conditionally changes vertical, along-rail and across-rail velocity components. Along/across modifications are applied only when the native launch velocity is consistent with the current travel direction and speed. Unmatched or stale launches are recorded but left unchanged.

**Threading and lifetime:** physics state and travel samples are physics-thread-owned; client tick publishes the skater's right axis; plugin shape configuration is shared under mutex. Hook control is process-lifetime because detours may still execute until process exit. The launch record list is bounded to 32.

**Compatibility requirement:** preserve caller return-address classification, state freshness window, board-body resolution, velocity vector layout, and physics-thread synchronization. The setter function match does not establish that the three callsites still call it or that their launch meanings remain the same. The `foot_gravity_prefix` is documented here only as a separate trainer contract; it is not a reason to reopen the earlier gravity-prefix investigation.

## 28. `grind_target.h` — temporary native rail-candidate selection override

Consumer: `Extension/Skater/grind_target.cpp`.

| Symbol / field | Contract | Compatibility significance |
|---|---|---|
| `select` / `select_prefix` | Native rail-candidate selection function, detour ABI `void(object, output)`. | Hook preserves original selection algorithm and only modifies a narrow temporary request window. |
| `context` | Object +0x98 points to skater context. | Must match the active local skater context. |
| `request_gate` | Object +0xaec float gate. | Must be finite and below 5 for override eligibility. |
| `alignment` | Object +0xb10 five-byte scratch/cache region. | Temporarily replaced with cached alignment bytes during original call and restored afterward. |
| `requested` / `request_flags` / `requested_mask` | Context fields +0x17d0 and +0x13dc; request bit 0x20000. | Temporarily sets desired grind type and request flag, then restores prior state if the native call left the injected value in place. |
| `root`, `target`, `targeting`, `direction` | Core +0x3b8 -> target object; target direction +0xc0 and targeting byte +0x1fb. | Eligibility check ensures the active target is valid and direction values are finite/bounded. |

**Eligibility and behavior:** override is only active for the owner of a 100 ms request lease, while gameplay input is allowed, overlay is closed, local skater/context match, physics state is 201, target is active, gate is sane, requested type is in [0,7], and all memory fields validate. It writes the requested type, request bit and five-byte alignment cache, forwards the original selector, reads the native-produced alignment result, then restores the previous bytes and request fields. Any write/restore failure clears ownership or cache so stale temporary state is not retained.

**Compatibility requirement:** this is not a replacement rail-selection algorithm. It is a scoped mutation around the native algorithm. A candidate must preserve object/context relationships, exact caller object layout, request mask semantics, native output cache behavior and restoration conditions. The 16-byte prefix is only a guard for the entry point; field offsets and the original call's side effects are equally important.


## 29. `game_states.h` — trainer access to the skater's state-page graph

Consumer: `Extension/Trainer/trainer_gamestate.cpp`.

| Symbol / field | Contract | Compatibility significance |
|---|---|---|
| `ant_driven_vtable`, `states_block_vtable` | Type checks for the skater's AntDrivenComponent and its state-page block. | The component and page block are linked through the animatable object; matching either vtable alone is insufficient. |
| `state_lookup` | Resolves a state's value from a state object and the context holding its pages; returns null when absent. | Trainer reads depend on the state object's handle and the correct context/page graph. |
| `state_write_lookup` | Resolves writable storage from context, state object and create flag. | Native write path; the trainer currently uses its own guarded memory access rather than relying on a replacement implementation. |
| `float_setter` | FloatAsset virtual setter; the state object is passed as the subobject at object +0x60 and the value is the third argument. | Its virtual-subobject convention and argument position are ABI-sensitive. |
| `state_copy` | Copies a value between state objects/contexts through the destination type's copy operation. | Relevant to state transfer semantics, not a general byte-copy primitive. |
| `rate_compute` | Computes a playback-rate controller's per-tick value, clamped to its asset's minimum and maximum. | Trainer time-scale work depends on the rate-controller behavior remaining compatible. |

**State graph layout observed by the consumer:** asset objects contain a 16-byte ID followed by the object; the name is at +0x18 and the packed state handle at +0x28. The handle selects a page (low 12 bits), a value slot (next 11 bits), and a presence bit in the page map at +0x71c. Handles are read from the live objects because they vary by level. The skater's AntDrivenComponent exposes its animatable at +0x68; two observed pointer paths lead to the state block, and page n is at block +0x60 + n*8.

**Compatibility requirement:** preserve the page graph, handle decoding, object identity and per-level handle discovery. The listed function prefixes are entry-point evidence, not proof that the entire state-object layout is unchanged. Trainer reads/writes are guarded and should remain fail-closed when the graph cannot be validated.

## 30. `game_state_values.h` — animation graph named-value setter

Consumer status: no direct include of this header was found in the inspected runtime source set. It documents a separately observed animation-graph write path.

| Symbol / field | Contract | Compatibility significance |
|---|---|---|
| `int_setter` | Setter for integer and enum graph values; call shape documented as (setter descriptor, graph instance, value). | The descriptor is resolved from the value descriptor plus +0x60; the graph instance owns the destination storage. |
| `descriptor_name` | Name pointer at descriptor +0x18. | Used to identify a specific graph value by its asset name. |
| `pop_height_name` | `Animation/Dingo/EnumGS.PopHeight`. | Semantic identifier for the game's low/medium/high pop grade, not a function signature. |

The observed setter is invoked by graph jobs on worker threads. A hardware write watch identified the value; a measured standing quick-flick experiment associated values 1, 2 and 3 with low, medium and high pop grades. This is an observation of the reference build, not evidence that the current SDK calls the setter or that the grade thresholds are universal.

**Compatibility requirement:** treat this as a research contract until a concrete consumer is added. Preserve descriptor-to-setter resolution, graph-instance ownership and worker-thread assumptions; do not infer that the pop grade is equivalent to launch velocity or a trainer-defined pop category.

## 31. `skater_body.h` — per-step ragdoll contact records

Consumer: `Extension/Skater/skater_physics.cpp`, with the public plain-data representation in `Engine/Game/Skater/skater_body.h`.

| Symbol / field | Contract | Compatibility significance |
|---|---|---|
| `contacts_before_skeleton_contract` | The skater core physics step processes body contacts before invoking the skeleton response. | Contact data read during the skeleton response belongs to the current physics step. |
| `contact_holder_offset`, `contact_struct_offset` | Rig +0x2f10 points to a holder; holder +0x1040 is the contact structure. | Both pointer relationships must remain valid. |
| `bone_records_offset`, `bone_record_size`, `body_bone_count` | Records begin at contact +0x5e0; 24 records of 0x70 bytes. | Bone ID order and record stride are part of the read contract. |
| `bone_peak_offset`, `bone_tracked_peak_offset` | Peak impact speeds at record +0x50 and +0x54. | Ordinary and tracked-point contacts use separate peak fields. |
| `bone_normal_offset`, `bone_point_offset`, `bone_slide_offset` | Normal, contact point and normal-cross-relative-velocity data. | The slide vector's magnitude represents speed along the surface; vector interpretation must not be swapped. |
| `bone_hit_*_offset`, `bone_touching_offset` | Per-bone hit-kind flags and a touching byte at contact +0x340. | Used to distinguish board, vehicle and world contacts and current-step contact presence. |
| `body_transform_contract`, `body_transform_offset` | Physics getter copies a body's 64-byte world matrix from body +0x20. | Separate transform contract; the contact record does not contain the full body transform. |

The contact processor clears one 0x70-byte record for each of 24 physics bones at the beginning of each step. It tracks the strongest impact along the contact normal, contact normal, contact point and a cross product of normal with relative velocity. The latter's length is interpreted as surface-slide speed. Two tracked-point variants are maintained for selected contacts. Bone 0 is the board root and is not a ragdoll body hit; the remaining 23 IDs follow the physics-bone name map.

The public representation maps those records into `Contacts::bodies[24]`, with touching, impact, slide, hit-kind flags and optional point/normal. Feet are deliberately excluded from the tumbling heuristic because they contact the board and ground during ordinary riding.

**Compatibility requirement:** retain same-step ordering, 24-bone index mapping, 0x70 stride, contact-structure offsets and the meaning of the normal/slide vectors. The fingerprints cover selected producer instructions; they do not establish that the entire record schema or all hit-kind meanings are unchanged.

## 32. `skater_entities.h` — native entity creation, placement and component operations

Consumers: `Extension/Skater/ai_skaters.cpp` and `Extension/Multiplayer/Remote/native_skater_spawn.cpp`; component sampling also uses its secondary skater-component vtables.

| Symbol / field | Contract | Compatibility significance |
|---|---|---|
| `blueprint_type`, `object_blueprint_type` | Distinct TypeInfos for skater blueprints and world-object blueprints. | These are different branches of the type hierarchy and must not be conflated. |
| `descriptor_init`, `descriptor_destroy` | Initialize a 0x190-byte creation descriptor in place and destroy its internal list state at descriptor +0x10. | Descriptor size, construction and cleanup must match the native creator. |
| `create_entity` | Creates an entity from result storage, descriptor, blueprint and two zero arguments; result includes entity and retained sublevel/reference data. | Return structure and retained-reference ownership matter, not only the function entry. |
| `destroy_entity`, `release_reference` | Destroy entity with owner; release one reference on an entity bus or retained sublevel. | Destruction ownership and reference balance must remain correct. |
| `place_entity`, `move_spatial_entity`, `initialize_placement` | Entity vtable placement slot +0x158; generic spatial transform setter; component initialization lifecycle. | Placement copies transform data and notifies components; initialization is not interchangeable with a raw transform write. |
| `customization_component`, `set_customization_flag_prefix`, `source_apply_appearance` | Resolve customization component and apply appearance state. | Appearance setup depends on component identity and the native source's mode semantics. |
| `physics_c7_setter`, `physics_cb_setter` | Skater-component physics flag setters for component bytes +0xc7 and +0xcb. | Used when constructing remote actors; preserve byte semantics and initialization order. |
| `skater_component_base_vtables` | Secondary base vtables at component +0x38 and +0x40. | Component validation in the skater model sampler checks both, in addition to the primary vtable. |

**Consumer flow:** AI and remote-skater creation both build the native descriptor, invoke `create_entity`, retain/validate the resulting entity, initialize placement and appearance, and later destroy the entity or release retained references. AI spawning additionally configures brain/physics state and verifies the source binding path. Remote spawning also creates or repairs associated board entities and validates lifecycle reachability before destruction.

The object-blueprint mesh chain is separately documented in the header: blueprint +0x80 to object data, object data +0xd8 to mesh asset, mesh asset +0x70 to loaded mesh, and loaded mesh +0x28 to min/max bounds. These are read-only descriptive asset-layout observations, not part of the skater-entity creation ABI.

**Compatibility requirement:** entity creation is a multi-symbol and lifetime contract. A candidate must preserve descriptor construction, result layout, entity/owner relationship, placement lifecycle, component vtables, customization initialization and retained-reference release. A high-confidence match for `create_entity` alone would not establish a safe spawn path.

## 33. `skater_state.h` — live skater, offboard-state and physics-body layout

Consumers: `Extension/Skater/skater_movement.cpp`, `Extension/Skater/skater_physics.cpp`, `Extension/World/owned_bodies.cpp`, and related no-bail/noclip readers.

| Symbol / field | Contract | Compatibility significance |
|---|---|---|
| `physics_state_offset` | Context +0x1414, uint32 selected physics state. | State values are interpreted alongside the native state machine, not as standalone booleans. |
| `step_seconds_offset` | Context +0x17ec, float simulation-step duration. | Shared with the skater core step-input publication. |
| `trick_state_offset`, `offboard_state_offset` | Core +0x3b8 points to trick state; +0x78 points to offboard state. | Pointer chain is required before reading ragdoll/on-foot flags. |
| `ragdoll_offset`, `in_the_air_offset` | Offboard state +0x18c and +0x180, byte flags. | Distinguishes ragdoll and airborne substates. |
| `board_holder_offset`, `board_physics_offset`, `rig_physics_offset` | Core +0x430 -> board physics at +0x18; core +0x438 -> rig physics at +0x2f10. | Board and ragdoll bodies live in different collections. |
| `physics_bodies_offset`, `physics_body_size` | Body array at physics object +0x20, stride 0x130. | Used to resolve body n and its velocity/spin. |
| `body_owner_offset`, `body_velocity_offset`, `body_spin_offset` | Per-body owner +0x10, linear velocity +0x70, angular velocity +0x90. | Owner and motion fields are distinct. |
| `body_pending_offset`, pending flag constants | Pending flags at body +0x60; rotation 1, position 4, velocity 8, spin 0x20. | Native synchronizer consumes these flags to publish state into the physics world. |
| `core_steps_since_built` | Core +0x190, count of physics steps since last core build. | Useful generation-like evidence across spawn/teleport, but not a pointer identity. |
| `core_jump_block_offset`, air-state and request offsets | Jump block/core air-state relationship and frame/dt fields. | Flight timing is built into the request block and can differ from current step duration. |
| `context_sensor_* ` | Observed sensor fields between context +0x1a00 and +0x1c40. | Meanings are explicitly unresolved; retain as observations only. |

The board physics collection has nine bodies, with body 0 the board root. The rig collection has 26 bodies, with the ragdoll body's indices corresponding to the physics-bone map. Each body wrapper is 0x130 bytes. The native synchronizer consumes pending flags to copy velocity, spin, position or rotation to the world body. The core's step count advances once per physics step in the observed runs and remains at the same core address through teleports.

**Compatibility requirement:** preserve the context/core/rig/board pointer chain, collection counts and stride, body owner relationship, motion offsets, pending-flag semantics and state-machine checks. Do not assume the jump block's stored dt follows a changed simulation-step duration. The sensor block is unresolved and must not become a hard matcher constraint.

## 34. `physics_bodies.h` and `rigid_bodies.h` — world body/shape handles and owned-body API

Consumer: `Extension/World/owned_bodies.cpp`; related read paths include park-editor surface probes and world body/shape identity helpers.

| Symbol / field | Contract | Compatibility significance |
|---|---|---|
| Native handle | 16-byte handle `{world pointer, index, generation}`. | Every operation must validate the world, allocation and generation; an index alone is unsafe. |
| `store_offset` | Physics world +0x28 points to the store. | Root of the body/shape state arrays. |
| `body_owner_offset`, `owner_kind_offset`, `owner_value_offset` | Per-body owner index at +0x1008; owner-kind bitset +0x8b0 and owner value +0x8b8. | Used to identify entity-owned physics and distinguish ECS owners. |
| `ghost_body`, `solid_body`, `main_ghost`, `main_solid` | Native ghost/solid bit operations. | The object main-body bit and individual body bits are distinct paths. |
| `linear_force`, `angular_torque`, `set_inertia_ratio` | Native force, torque and inertia-ratio operations. | Their body state and vector arguments must match the native API. |
| `synchronizer` | Game body synchronizer; observed producer phase for force/torque updates. | Local phase evidence only, not a global physics barrier. |
| `overwrite_force`, `overwrite_torque` | Replace pending force or torque for dynamic bodies. | Not interchangeable with the general linear/angular velocity setters. |
| `set_motion_type`, `create_capsule`, `set_categories`, `set_collides_with`, `set_shape_owner` | Operations used to create and configure the remote/owned collision shapes. | Body and shape handles, category masks, owner slots and motion type must remain coherent. |
| `set_transform`, `set_linear_velocity`, `set_angular_velocity`, `set_center_of_mass`, `wake_body` | Native body state operations. | Argument layouts include vec4 or eight-float transform data; motion-type-specific behavior matters. |
| `body_valid`, `shape_valid` | Check allocation and generation. | Required before using retained handles. |

**Shared layout:** the store contains body/shape allocation and generation arrays, shape-to-body indices, category masks and dynamic-body state arrays. `rigid_bodies.h` documents offsets for position/rotation/velocity rings, pending force/torque, inverse mass, inertia ratio, allocated/dynamic bitsets and shape mappings. Shape owner slots use 0xffffffff for no owner. The optional shape-category reader is separately fingerprinted and may be disabled without disabling the rest of the feature.

**Important terminology:** the physics engine's native API calls the 16-byte handle a body for rigid-body operations and a shape for collision geometry. The store maps a shape index to its carrying body. Do not collapse those identities when adapting a candidate build.

**Owned-body safety contract:** `owned_bodies.cpp` anchors operations to the current client, game context, entity, core, skater context, rig, world and physics state. It captures and rechecks relevant memory, validates handles and generations, observes native body ownership, and tracks setter failures/quarantine. The synchronizer fingerprint only qualifies the observed producer phase. The service must not treat it as exclusive ownership or a world-wide synchronization point.

**Compatibility requirement:** this is one of the highest-coupling contracts in this batch. Matching a setter does not establish handle layout, store offsets, allocation/generation semantics, owner mapping or synchronization safety. Keep body and shape identities, ownership, world lifetime and per-operation validation as a single dependency group.

## 35. `remote_collision.h` — remote-player collision capsules

Consumer: `Extension/Multiplayer/Remote/remote_collision.cpp`.

| Symbol / field | Contract | Compatibility significance |
|---|---|---|
| `game_context_global`, `physics_world` | Resolve the current client physics world from the game-context global. | Null while level physics is unavailable; cached handles must be scoped to the world. |
| `allocate_body`, `create_capsule` | Allocate a body and create a capsule shape attached to it. | Native output handles are 16-byte world/index/generation structures. |
| `set_motion_type`, `set_body_980`, `clear_body_aa0` | Configure the proxy body as keyframed and apply the same initialization values as the retail remote-skater proxy. | Initialization order and body flags are part of the contract. |
| `set_categories`, `set_collides_with`, `set_shape_owner` | Configure collision masks and owner slot. | Remote proxy category 21 collides with CharacterProxy and LocalSkater categories; owner is explicitly none. |
| `set_center_of_mass`, `set_transform`, `set_linear_velocity`, `set_angular_velocity`, `wake_body` | Place and move proxy bodies. | Transform is quaternion plus position in an eight-float array; velocities are vec4-like inputs. |
| `body_valid`, `shape_valid` | Validate retained body/shape handles. | Prevents stale or recycled handles from being operated on. |
| `core_party_collision` | Local skater core +0x40e1 party-collision byte. | Remote collision is enabled only when the game's local party-collision setting permits it. |

The feature creates two keyframed capsules per remote player: an upright skater capsule and a thin board capsule. The skater capsule is positioned at the remote root; the board capsule follows the board pose only while it remains within the configured reach. Velocity is estimated from successive positions over a bounded time interval so moving proxies can impart motion while teleports do not become impacts. Categories are enabled only while the remote player is considered solid; otherwise the shapes are made inert.

Per-peer slots are scoped to the physics world and local context. On a world change, old handles are not touched in the new world. If the same world is reused with a different context, the old proxies are switched off before the slot is reset. A player not updated for 250 ms has its capsules disabled. Failed creation is retried after five seconds. The local party-collision check validates the skater entity, skater component and core vtables before reading the setting.

**Compatibility requirement:** preserve the handle ABI, world lifetime, keyframed-body setup, capsule geometry, category masks, owner semantics, transform convention, stale-player expiry and local party-collision gate. This feature depends on the physics-body and rigid-body contracts in section 34; a standalone match of the capsule constructor is insufficient.

## 36. `skater_movement.h` and `skater_model.h` — local skater telemetry readers

Consumers: `Extension/Skater/skater_movement.cpp` and `Extension/Skater/skater_model.cpp`.

| Header / field | Contract | Compatibility significance |
|---|---|---|
| `skater_movement::context_velocity` | Context +0x3d0, vec3 world velocity. | Read only after local skater/context validation and finite/range checks. |
| `skater_parent`, `skater_board` | Core +0x3b0 parent state machine and +0x430 board holder. | On-foot walking state is resolved through the parent machine, not inferred from velocity alone. |
| `state_context`, `state_board`, `state_rig`, `active_state` | Parent machine references to context, board, rig and active substate. | Cross-checks must agree with the current local skater. |
| `walking_velocity` | Active walking substate +0x10, vec3. | Used only when the active state is the expected walking state and the player is grounded. |
| `component_globals` | Three global addresses sampled for presence by `read_skater_component`. | Presence observations only; not evidence of the globals' internal types or behavior. |
| Component vtable checks | Primary skater-component vtable plus secondary base vtables at +0x38 and +0x40. | The sampler validates the full observed component identity before exposing its fields. |

The movement reader obtains the local skater through the no-bail resolver, reads and sanity-checks context velocity, then cross-checks the physics state, trick/offboard state, ragdoll and airborne flags, parent machine, board, rig and active walking-state vtable before reporting grounded walking velocity. Input-derived stick direction additionally requires foreground focus, no free camera, no menu and no park-editor placement ownership. It is projected through the current local camera.

The model reader is a read-only process-memory sampler. It reads a 0xd8-byte skater component snapshot, validates the primary and two secondary vtables, reports presence of selected component pointers and three globals, samples component flags and wait time, and optionally checks a nested target pointer. The wait value is required to be finite.

**Compatibility requirement:** these are guarded telemetry contracts, not native function hooks. Preserve the complete pointer/vtable cross-checks and finite-value validation. A matching offset without the identity chain must not be treated as a valid local skater sample.

## 37. `skater_skeleton.h` — installed-game skeleton and costume asset identity

Consumer: `Extension/Skater/xray_skeleton.h` and its skeleton asset reader.

| Symbol / field | Contract | Compatibility significance |
|---|---|---|
| `skeleton_toc`, `skeleton_bundle`, `skeleton_asset` | Installed-game TOC, bundle and render-skeleton asset path. | Asset identity and bundle location, not an executable function ABI. |
| `mesh_toc`, `mesh_bundle`, `mesh_asset` | Dem Bones costume TOC, bundle and skinned-mesh asset path. | Separate costume mesh resource that is skinned to the render skeleton. |
| `mesh_lod` | LOD index 4, observed at 13,817 triangles and 12,222 vertices. | Asset-content assumption used by the CPU-side pose/draw path. |

The render skeleton contains 386 joints. The x-ray skeleton reader uses the skeleton asset and a corresponding joint-name mapping to pose the costume mesh from the game's skin transforms. The mesh is a separate asset with six LODs; the selected LOD is the fifth entry (zero-based index 4).

**Compatibility requirement:** verify asset paths, skeleton joint identity/order, skinning data and selected LOD independently of executable function matches. A path remaining present does not prove the asset's contents or joint mapping are unchanged.


## 38. `native_audio.h` — capture and replay of native skater sound frames

Consumer: `Extension/Multiplayer/Remote/native_audio.cpp`; supporting frame layout in `native_audio_layout.h` and actor/handle resolution in `native_audio_binding.h`.

| Symbol / field | Contract | Compatibility significance |
|---|---|---|
| `submit` | Skater sound component submit hook, arguments (handle, frame). | Hook observes the game's normal sound update; original local submissions are forwarded unchanged. |
| `frame_copy` | Merges selected fields from a partial frame into owned storage using masks. | Audited as a primitive-field merge; it is not a general object copy or allocator. |
| `frame_difference` | Native frame-difference routine, reference only. | Used as corroborating evidence for the frame layout, not called by the current consumer. |
| `frame_reset` | Resets a sound frame to native defaults. | Called when local capture ownership changes. |
| `stop_sound` | Stops a sound component and clears its handle at component +0x74. | Used for teardown; the native component owns the handle and must stop it through its own operation. |

**Frame contract:** `NativeAudioFrame` is 16-byte aligned and 0x220 bytes. The consumer captures a 0x218-byte native update, merges the selected fields into persistent storage, then converts only portable values into the multiplayer `AudioState`. Matrices, native handles, asset pointers, padding and dirty masks are not sent over the wire. The layout header records 49 float offsets, 26 selector offsets and 18 material-selector offsets. Material words encode flags and material/property slots; slots outside the 116 root-level materials are normalized to the default material because Studio-map material indices are install-specific.

**Capture and playback flow:** the submit hook suppresses only default submissions from registered remote visual-only actors. It always forwards the local player's native sound update, then snapshots the frame if component/entity ownership and the handle at component +0x74 validate. Local capture is client-thread-bound. For remote playback, the actor's audio component and source binding are fully resolved at one-second intervals, with lightweight owner/handle checks between resolves. The remote frame is rebuilt from transmitted audio values and the buffered root/board transforms, then submitted through the original native function. Teardown calls the component stop operation; it does not free or reuse the handle itself.

**Compatibility requirement:** preserve the 0x220 frame representation, field offsets, dirty-mask merge semantics, material portability rules, component +0x74 handle ownership, source binding and client-thread assumptions. A match for `submit` alone cannot establish that the native frame is still structurally or semantically compatible.

## 39. `native_cosmetics.h` — remote appearance recipe copy and material parameter APIs

Consumer: `Extension/Multiplayer/Remote/native_cosmetics.cpp`; native recipe and catalog layout in `native_cosmetics_layout.h`. Related material manipulation also appears in `Extension/Skater/layer_materials.cpp`.

| Symbol / field | Contract | Compatibility significance |
|---|---|---|
| `cosmetic_component_vtable` | Customization component whose recipe is at +0x150. | Component identity must be checked before reading or replacing the recipe. |
| `recipe_copy` | Deep-copies a native recipe into a component. | The current consumer relies on native ownership transfer of strings and arrays; borrowed packet storage must remain alive through the call only. |
| `publish_materials` | Publishes an AppearanceItem's named material map to its render meshes. | A material parameter write does not become visible until the owning item publishes. |
| `set_shader_parameter` | Typed per-instance shader parameter setter (material, ParamDbKey*, NativeValue*, priority). | Uses the engine allocator and preserves native priority rules. |
| `color_rgb_type`, type hashes | Type descriptor and hashes for ColorRgb, float, integer and boolean material values. | Type hash, size and descriptor index must agree before constructing typed values. |
| Material parameter map offsets | Hash map at material +0x268; bucket count +0x270; count +0x274; node value +0x10, type +0x20, flags +0x28, next +0x40; dirty byte +0x2f6. | Layout and dirty/publication sequence are required for direct guarded inspection or material updates. |
| `named_lookup` | FNV-1a name lookup in a loaded table; fifth argument is passed on the stack. | The native lookup dereferences the table without a null check; callers must validate preset availability. |
| `add_shader_preset` | Lua binding for item:addEsShaderPreset(material, preset). | The preset argument is dereferenced without validating that the resource loaded; missing preset is a crash condition. |
| `script_preset_reference`, `referenced_preset`, `script_userdata` | Lua userdata/reference extraction and resolution of the preset object. | Lua argument position, reference lifetime and native object resolution are part of the call chain. |
| `item_material`, `material_preset_name`, `material_item`, `material_preset` | Helpers to obtain/create a named material and associate its preset name, owning item and Lua reference. | These operations establish ownership and association; they are not interchangeable setters. |
| Preset layout | Parameter array at preset +0x20, 40-byte records; boxed value at +0, Parameter +0x10, ParamKey +0x18. | Applying a preset type-checks populated values and reads the referenced TypeInfo. |

**Recipe representation:** the native recipe is 0x20 bytes, with pointers to scalar values, item records and a mask, followed by a word and flag. Each item is 0x18 bytes and carries an asset string pointer, parameter pointer, hash and slot. The remote consumer builds temporary borrowed arrays from the received recipe, validates bounds and slot/template compatibility, calls the native deep-copy routine, then verifies the component's dirty/update flags and that the native component no longer points at the borrowed item array.

**Application flow:** the target and local entities must be distinct actors of the same type and share the same cosmetic template resource. Local-player components are rejected. Slots are fitted by slot ID; missing local slots use local parameter shapes, and unsupported remote items are replaced with policy-selected stand-ins, defaults or empty slots. Reserved store items and plugin-hidden items are blocked from being applied. After the native copy, the game's normal dirty-flag path performs appearance/mesh updates.

**Material/preset caveat:** the header also documents a lower-level shader-material/preset chain. The multiplayer recipe consumer directly depends on `recipe_copy`; it does not thereby prove every shader/preset helper is used by that path. Keep the material/preset subcontracts separately classified until their exact consumers and callsites are traced.

**Compatibility requirement:** preserve recipe structure, deep-copy ownership transfer, component dirty flags, template/slot validation and the catalog's hash/category representation. Do not promote the material parameter map or preset helpers to behaviorally confirmed merely because `recipe_copy` matches.

## 40. `native_vfx.h` — remote skater effects and transmitted contact impacts

Consumer: `Extension/Multiplayer/Remote/native_vfx.cpp`.

| Symbol / field | Contract | Compatibility significance |
|---|---|---|
| `register_skater` | Registers an effects component with the effects manager by key. | Hook records the manager key/component relationship while forwarding native registration. |
| `impact` | Plays one world-contact effect from manager, key and impact record. | Hook forwards the native effect, then captures the contact values for multiplayer transmission. |
| `refresh` | Stops and rebuilds a skater's effects from settings under the manager's lock. | Required after remote outfit changes. |
| `build_effects` | Builds effect sets for an entity from its settings, id and flags. | Hook relocates always-on wheel effects from the skater to the skateboard when they fit its rig. |
| `joint_index` | Resolves a joint ID in a rig's skeleton map. | Native tree walk is unbounded; consumer performs a bounded preflight walk before calling it. |
| `thread_world`, `manager_offset` | Resolve the calling thread's world and effects manager. | The manager is world/thread scoped; a global cached manager is not a safe substitute. |
| `materials_offset` and grid fields | Resolve the level material grid and its index table. | The native effect path reads material indices without a bound; the consumer validates the material first. |
| Component and skeleton offsets | Effects component vtable/size, owner, rig-owner, rig skeleton and joint-tree offsets. | Used to prove that a registered component belongs to the current remote skater or board. |
| Impact record | 0x40 bytes: position at +0, velocity +0x10, normal +0x20, material +0x30. | The transmitted impact contains only validated portable values, not native pointers. |

**Behavioral flow:** the local game's native impact call is forwarded first, then the consumer captures position, velocity, normal and material. Material indices are checked against the current world's material grid; default or previously observed safe materials are handled separately. The wire impact is validated and kept in bounded recent/drain queues. Remote impacts are played through the native effect function for the current peer slot.

The game creates remote skater and board effects before the remote outfit is fully applied, so the consumer requests a refresh after outfit changes. Wheel effects that are always-on and whose joints exist on the board rig are moved from the skater's effect set to the board's. Effects tied to transient skater state remain on the skater. The consumer verifies ownership of components and checks that the joint-tree lookup terminates before invoking the native unbounded lookup.

**Compatibility requirement:** preserve impact record layout, material-grid interpretation, thread-local world resolution, manager registration keys, component ownership, rig/skeleton relationships and effect-set trigger semantics. A match of the impact function does not prove the registration/build/refresh chain or the material table is compatible.

## 41. `native_voice.h` — native spatialized voice playback

Consumer: `Extension/Multiplayer/Voice/native_voice.cpp`.

| Symbol / field | Contract | Compatibility significance |
|---|---|---|
| `sound_create` | Creates a sound from a voice asset and returns/sets a native handle. | Requires the correct sound manager and loaded voice asset. |
| `sound_destroy` | Destroys a native sound handle. | Must be paired with sample-source removal during cleanup. |
| `sound_set_input` | Binds typed sound inputs to a handle. | Used for the source ID and spatial transform; type descriptor and input argument ABI matter. |
| `samples_submit` | Submits signed 16-bit PCM samples, sample count and source ID to the provider. | Sample format, count and source identity must match the native provider. |
| `samples_remove` | Removes a source ID from the provider. | Required to release queued/provider-side samples when a sound is removed. |
| `sound_manager`, `sound_manager_vtable` | Global manager pointer and expected manager vtable. | Revalidated during playback and before teardown. |
| `samples_provider` | Global PCM sample provider. | Provider identity is rechecked before removing samples. |
| `spatializer`, `voice_asset_vtable` | Global spatializer; voice asset at spatializer +0x228. | The asset is checked by vtable and its 16-byte identity before use. |
| `source_input_type`, `transform_input_type` | Native input TypeInfo pointers for source and transform inputs. | Source ID and 4x4 transform must be supplied through the expected typed inputs. |

**Playback flow:** the implementation validates all five function prefixes once and binds itself to the image and client thread. It then checks the manager/provider globals, their vtables and the voice asset identity. Per remote player, it creates a native sound, binds a tagged 64-bit source ID, updates a 4x4 spatial transform, and submits 16 kHz signed 16-bit PCM in 20 ms blocks. Request state expires after 300 ms without updates; stale PCM is flushed or discarded, and the provider queue is kept below a bounded horizon. Source IDs use a distinct high-word tag to avoid collisions with other plugin/native sources.

Manager, provider or asset changes trigger cleanup. Sound handles are destroyed only while the current manager still matches the manager that created them; provider samples are removed only while the provider identity matches. Cleanup failure disables readiness rather than continuing with potentially stale native objects. Generation and serial checks prevent delayed packets from reviving an older peer voice stream.

**Compatibility requirement:** preserve function signatures, manager/provider identity, voice asset identity, typed input semantics, PCM format, source-ID namespace, client-thread restriction and cleanup ordering. The five function prefixes are an admission gate, not proof that the sound manager, provider or asset schema is unchanged.


## 42. `startup_interventions.h` — one-shot VM frame capture and startup exit intervention

Consumer: `Extension/Boot/startup_interventions.cpp`; state-machine interface in `Extension/Boot/startup_interventions.h`.

| Symbol / field | Contract | Compatibility significance |
|---|---|---|
| `frame`, `frame_instruction`, `frame_fingerprint` | VM byte-store site at RVA 0x08ae2785; target instruction is `MOV [RDX], SIL`. | Breakpoint is installed at an instruction that also performs ordinary byte stores; the exception handler must emulate those stores exactly when the target frame is not being captured. |
| `frame_data`, `frame_cursor`, `frame_count` | Expected bytecode address, cursor address and frame-count marker. | These values identify the specific VM clear operation rather than every byte-store execution at the site. |
| Frame stack/VM/register offsets | RBP-relative stack layout, VM pointer, saved-register pointer and cursor. | The handler validates a full register/stack shape before copying the 0xc0-byte frame. |
| `exit_thunk`, `exit_return`, `exit_instruction` | Startup exit thunk and exact return address/instruction. | Second breakpoint is accepted only for the captured frame, same thread and same RBP/RSP relationship. |
| `StartupInterventionMachine` | One-shot phases: uninitialized, armed, handling_frame, frame_captured, handling_exit, complete, failed. | Enforces capture-before-exit and prevents a mismatched thread/frame from restoring arbitrary memory. |

**Execution flow:** installation validates the preferred image base, image size, executable-page ranges and both byte fingerprints. It registers a vectored exception handler, arms the exit breakpoint and frame-site breakpoint, then arms the state machine. At the frame site, the handler validates register relationships, cursor and bytecode, saves exactly 0xc0 bytes, and lets the native clear execute. Ordinary stores at the same site are replayed as `MOV [RDX], SIL` without changing flags/registers. At the exit thunk, the handler validates the captured thread, stack frame, return address, instruction and expected register values, restores the saved frame, sets RAX to FALSE and resumes at the guarded return address. Breakpoints are retired as the sequence completes.

**Compatibility requirement:** this is an instruction-site and exception-context contract, not a conventional function hook. Exact image base, instruction bytes, register values, stack offsets, thread identity and ordering are all required. A function matcher that only relocates the two sites cannot establish safety. Any mismatch must fail closed rather than restore a frame using partially matching state.

## 43. `entity_pages.h`, `spatial_buckets.h` and `pending_updates.h` — coordinated large-world capacity patches

Consumer: `Extension/World/native_entity_pages.cpp`, which combines all three patch tables and validates every expected instruction before writing any replacement.

| Header / field | Contract | Compatibility significance |
|---|---|---|
| Entity TransformSpace/property-space pages | 128 page pointers, 2,048 entries per page, 16 KiB per page, capacity 262,144 slots. | Retail capacity was 131,072; a 150,152-entry map load exceeded it and overwrote snapshot bookkeeping. |
| Render-instance pages | 512 page pointers, 2,048 records per page, 0x1d0-byte records. | Enlarges leaf pages while preserving stable object addresses and the native page-pointer count. |
| Spatial bounds arena | Minimum 128 MiB, preserving any larger configured value. | Must be sufficient for city-scale cull IDs and bounds while retaining native allocation/reset ownership. |
| Spatial buckets | 16,384 blocks per bucket, 64 entries per block, nine root nodes. | Every bucket array grows together; all native offsets and root stride are asserted in the header. |
| Pending-update queues | 24-byte entries; 2,048 entries per page, 0xc000 bytes per page. | Transform/property snapshots and pending updates must use matching page capacity. |
| Patch tables | Instruction-level changes to index shifts, page byte/entry counts, spatial bucket strides/layout, render-instance page sizes and pending-update page calculations. | Every affected consumer site must be patched as one compatible set. |

**Patch application:** `native_entity_pages.cpp` concatenates the entity-page, spatial-bucket and pending-update patch arrays. It validates the PE image and every expected byte sequence before applying the first write. Each write changes page protection, writes the replacement, restores protection, flushes the instruction cache and verifies the result. If a write fails, all already-applied patches are restored in reverse order; failure to restore triggers fail-fast termination. Installation is bound to one image base.

The three headers deliberately share `entity_pages::Kind`, `Patch` and the consteval patch constructor. The patch lists include insertion/lookup, iteration, copy-on-write, snapshot repair, queue sorting, synchronous/asynchronous consumers, rounded iteration bounds and sized deallocation. This is not merely changing one capacity constant: all index shifts, allocation sizes, byte strides and loop bounds must agree.

**Compatibility requirement:** retain native ownership, IDs, snapshot structures, stable direct object addresses and queue entry layout. Candidate compatibility requires the entire combined patch set to resolve and apply coherently. A subset of matching instructions is not a valid partial implementation because it can leave readers and writers using different page geometry. Capacity 262,144 is based on the largest observed map plus headroom; larger maps would require revisiting the limit.

## 44. `native_patch_support.h` — selective unsigned native package-envelope acceptance

Consumer: `Extension/Assets/native_patch_support.cpp`.

| Symbol / field | Contract | Compatibility significance |
|---|---|---|
| `verifier` | Native data-envelope verifier at RVA 0x041fb4c0; complete 392-byte body fingerprint. | The hook preserves the native verifier for all inputs except the specifically recognized unsigned patch envelope. |
| `signature_import` | IAT slot for BCryptVerifySignature. | Validated against the actual loaded bcrypt.dll export, not the address of the SDK's local linker thunk. |
| `signature_return` | Return address of the verifier's BCryptVerifySignature call. | The signature detour only applies its special behavior at this exact native callsite. |
| `crypto_thunk`, `reader_call`, `layout_call`, unwind records | Supporting call/thunk/unwind byte contracts. | The complete verifier's surrounding control-flow and unwind assumptions are validated before installation. |
| `verifier_bytes` | Full 392-byte native verifier body. | Longer than a prefix; guards the exact envelope parsing and native return convention. |

**Envelope behavior:** the native verifier accepts a raw envelope and a pointer to its length. For a recognized unsigned native patch envelope, the hook validates the envelope header, bounds, pointer non-aliasing and writable/aligned length storage, then atomically subtracts the 0x22c-byte envelope header from the length and returns the payload pointer. It does not write into the source buffer. Other envelopes are passed to the original verifier.

The signature hook is scoped to the exact signature buffer and current thread so that the unsigned-patch exception cannot leak into unrelated BCryptVerifySignature calls, nested verification or concurrent engine work. Native crypto/provider errors and non-target signatures retain native behavior. Both hooks are enabled in one Detours transaction; if setup fails, the prepared hooks are removed and stored trampolines are cleared.

**Compatibility requirement:** preserve the full verifier bytes, envelope header size and classification, length-pointer alias/protection checks, atomic length commit, signature callsite identity and thread-local scope. This is a narrow trust-policy exception, not a blanket bypass of native signature verification.

## 45. `reference_lists.h` — removal of empty asset references before level registration

Consumer: `Extension/World/reference_list_guard.cpp`.

| Symbol / field | Contract | Compatibility significance |
|---|---|---|
| `register_named` | First reference-list loader at RVA 0x557c40. | Hook filters the list before forwarding to the native loader. |
| `register_assets` | Second reference-list loader at RVA 0x891c80. | Separate loader path; both must be guarded for complete coverage. |
| `asset_references` | Asset +0x28 points to its reference array. | The array count is stored immediately before the first element. |
| `asset_name`, `asset_type` | Asset name at +0x18; type at +0x08. | Name is used for bounded diagnostics; type is contextual metadata. |
| `pointer_flag`, `array_count_flag` | Low pointer bit 2 and high count bit 31 are flags. | Must be masked while preserving the flag bits when compacting the array. |

**Observed failure and behavior:** both native loaders iterate asset reference arrays and dereference each reference at +0x48. A null/empty reference is not skipped by the native path and can crash level loading. The guard compacts non-empty references over empty ones, lowers the count while preserving the count flag, logs a bounded number of affected asset names, then forwards the original list. The two loaders are hooked independently; if either fingerprint or hook fails, the other may remain hooked, but the overall guard reports incomplete coverage.

**Compatibility requirement:** preserve pointer/count flag encoding, array ownership and element stride, and ensure compaction happens before either native loader consumes the array. This is a narrow data sanitation hook. It does not repair missing assets or validate all reference semantics.

## 46. `media_catalog.h` — offline replay-draft thumbnail catalogue

Consumer: `Extension/Settings/replay_drafts.cpp`.

| Symbol / field | Contract | Compatibility significance |
|---|---|---|
| `services_vtable` | Expected vtable for the game's backend-services object. | Prevents using the catalogue helpers on an unrelated service object. |
| `lookup` | Finds the registered `catalog` container from services. | Required before creating or accessing the user's catalogue. |
| `add_container` | Creates/registers the catalogue container. | Normally called by the services constructor among online registrations, which do not run in this offline flow. |
| `user_added` | Creates the user's catalogue after a sign-in listener event. | Called with a synthetic local user record when the game has not made a catalogue. |
| Container layout | Listener at +0x60; catalogue at +0x68. | Listener and catalogue pointers have distinct roles and lifetimes. |
| Catalogue layout | Folder engine string +0; entries +0x38; entry stride 0x130; ready byte +0x5c. | Used to count and observe saved draft entries. |

**Offline flow:** in a level or level-menu state, the consumer validates all three function fingerprints and the services vtable. It finds or creates the catalogue container, removes only old numbered folders whose manifest matches the expected replay-draft thumbnail shape, and invokes the native user-added path using a static local user object. The native constructor lists the folder and loads entries, but its posted ready task is not run in the offline environment. After five seconds, the consumer marks the catalogue ready itself, records the entry count and watches later count changes.

If the game already created a catalogue for its own signed-in user, the consumer uses that catalogue instead. Catalogue state is one-shot through waiting/loading/done/failed stages; invalid services, lost catalogue or unreadable fields fail the feature rather than continuing with stale pointers.

**Compatibility requirement:** preserve service/container/listener identity, function signatures, synthetic-user string layout, catalogue entry stride and ready semantics. The manual ready-byte fallback is specific to the observed offline task behavior and must be revalidated if the services/task execution model changes.

## 47. `game_data_files.h` — supported-build Data-folder size manifest

Consumer: `Launcher/launch.cpp`, `game_files_supported`.

The header is generated from Steam depot 3354751 manifest 4621099302092747785 and contains 98 relative paths with expected file sizes. It covers Data TOCs, CAS archives, language packages, level bundles, InitFS, layout and chunk manifest files. It is a static file-integrity manifest, not a runtime function ABI.

The launcher iterates the list and checks the installed files against the expected sizes before launching the game. A missing or truncated data file is reported as an unsupported/damaged game-data installation even when the executable itself passes the supported-build fingerprint.

**Compatibility requirement:** keep the manifest paired with the exact game build/depot manifest. File size detects missing/truncated files but does not prove file content identity; do not describe it as a cryptographic data-file verification.

## 48. `job_system.h` — worker spin configuration observations

Consumer status: no direct include of this header was found in the inspected source tree. The header records three build-specific data addresses used by the job-system performance research.

| Symbol / field | Contract | Compatibility significance |
|---|---|---|
| `spin_ticks` | Global timestamp-counter duration used by idle workers before sleeping. | Both observed worker loops read it live on each pass. |
| `spin_milliseconds` | Float configuration value at the job config +0x1a0; 0.25 ms in this build. | Used at job-system start to compute `spin_ticks`. |
| `thread_count` | Integer count of started job threads. | Diagnostic/configuration observation; no function ABI is declared here. |

The associated research identifies worker loop, run-until-done, job-pop and startup computation routines by RVA, but those are comments/research references rather than declared fingerprint contracts in this header.

**Compatibility requirement:** keep this as research-only address data until an actual runtime consumer and its read/write behavior are located. Do not use these addresses as hard matcher anchors or modify worker scheduling based only on the header comments.

## 49. `guid_table_lookup.h` — open-addressed GUID table lookup and missing-key hang

Consumer status: no direct runtime consumer was identified in the inspected source paths; the header records a native failure analysis and two function fingerprints.

| Symbol / field | Contract | Compatibility significance |
|---|---|---|
| `find` | GUID-table lookup at RVA 0x41d3a50. | Probes an open-addressed table using the first GUID dword. |
| `record_contract` | Record-resolution routine at RVA 0x41c7df0. | Called after a matching 16-byte key is found. |
| `count`, `slots` | Record count at table +0x7c; u16 slot array at +0x958. | Slot count is at least 16 and approximately 1.5 times record count, rounded to a power of two. |
| `source`, `key_of` | Source object at +0x18; virtual key extraction at +0xc0. | Each candidate record's 16-byte key is obtained through this virtual call. |
| `empty` | 0xffff sentinel in the slot array. | The observed search does not terminate on an empty slot. |

**Observed defect:** the probe loop terminates only when a matching key is found. A missing GUID can therefore walk empty slots indefinitely rather than returning not-found. Two hang dumps from 2026-10-17 associated this with a custom-map server and a Throwdown asset-pack GUID.

**Compatibility requirement:** this is a behaviorally important native algorithm, but the current header does not establish an active SDK hook or complete caller set. Preserve the distinction between a matching lookup function and a corrected lookup algorithm. A future compatibility fix must establish a bounded probe/termination condition and verify the table's capacity/probe invariants without assuming this exact implementation is already consumed by ReSkatePlus.
