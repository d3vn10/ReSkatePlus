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
| `prepare_toc` | Converts TOC data words to native byte order. | ABI: `void*(data)` in the header's call-site use (actual declaration is `void(void*)`). Called on bytes immediately after the 0x22c-byte file header before comparing format and swapping the live TOC backing. Mutates the supplied buffer in place. | P1, call ABI described |
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
