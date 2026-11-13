# ReSkatePlus build-specific dependency contract map

Build namespace: `dingosdk::game::build::v20260929`  
Repository snapshot: `d3vn10/ReSkatePlus`, branch `main`  
Scope: all 137 headers under `Engine/Game/Build/20260929/`.

## Purpose and interpretation

This is the ReSkate-side dependency inventory for planning compatibility work against another Skate executable. It records the contract ReSkate expects from the game, not merely the old RVA or the byte signature.

The contract categories are:

- **Call**: function entry or call-site contract. Record arguments, return value, callback behavior, and side effects.
- **Hook**: intercepted native call. Preserve the original call and the timing/ordering of ReSkate's pre/post work.
- **Layout/global/vtable**: data read or written through a global, object offset, TypeInfo, or virtual slot. The object identity, ownership, and lifetime are part of the contract.
- **Patch**: instruction/call-site mutation. Record exact instruction boundaries, incoming control flow, and the resulting behavior.
- **Asset/data**: keys, hashes, schemas, file manifests, bundle paths, or other non-callable data.

Priority:
- **P0**: game boot, client update, level transition/request, world/resource availability, or another dependency whose loss prevents general runtime or map transitions.
- **P1**: world simulation, asset/mod loading, skater/physics, multiplayer actor/session integration, or broad runtime correctness.
- **P2**: a substantial optional feature or subsystem.
- **P3**: diagnostics, performance tuning, cosmetic presentation, or nonessential metadata.

A priority is about ReSkate's dependency on the entry, not how easy it is to recover. A data/layout contract can be P0/P1. A function can be P3.

**Audit boundary:** this is a complete header-by-header inventory and contract synopsis. Header comments and declarations provide strong evidence for many contracts, but not every declaration has yet been independently traced from every ReSkate call site. Entries marked **caller trace needed** must not be treated as ABI-verified. The next matcher should consume only entries whose argument/return and side-effect contracts have been promoted to verified status. This map deliberately does not claim that every listed RVA has a proven replacement in the newer executable.

## Per-entry map

| Header | Ecosystem / contract ReSkate relies on | ReSkate-side input, output, or mutation | Priority |
|---|---|---|---|
| `ai_skaters.h` | AI skater entity creation, entity buses, property binding, physics source, AI/brain-tree components. | Creates child entity buses and acquires/releases references; writes source flags into skater property binding; supplies physics source data; relies on component factories, controller creation/run, and vtable updates. Reference-only addresses must not be promoted to callable APIs without tracing. | P2 |
| `animation_values.h` | Per-rider animation graph, frame source/context, state export, graph property descriptors and occupied slots. | Reads/writes graph instance values and descriptor metadata at fixed offsets; uses native/basis masks and type descriptors. Layout/data contract, not a standalone function dependency. | P1 |
| `atmosphere.h` | Fog, sky, and wind live-component property dispatch; reference-counted property assignment. | Maps property hashes to live component offsets, sizes, masks, groups, and texture/reference semantics; reference assignment retains new values and releases old ones. | P2 |
| `audio_core.h` | EA audio SndPlayer1 stream processing and command dispatch. | Sends command 4 with a filename at arguments +0x18; reads player voice state/rate/frame/channel fields and mixer write/read buffers. Used to observe/control streamed music playback. | P2 |
| `body_contact.h` | Physics contact-vector readers and point-motion arithmetic. | Supplies body/world handles and reads result positions/normals at result +0x18/+0x20; the five fingerprints are helper contracts. Validity, actor exclusions, world identity, and result lifetime are additional requirements. | P1 |
| `buildkit_limits.h` | Build Kit object persistence settings class. | Resolves the settings vtable and TypeInfo/lookup key. Data identity contract used by settings access; caller trace needed for exact fields consumed. | P2 |
| `camera_observer.h` | Native camera callback and callback object/vtables. | Observes callback arguments/object fields and native caller return address for camera diagnostics and transition-sensitive camera state. | P3 |
| `client_source_spawn.h` | Client skater source creation, free camera, view manager, UI settings, and noclip physics updates. | Uses source creation data; switches camera mode; writes camera transform; reads/writes camera/view manager and UI settings; hooks physics update and skater motion for noclip. | P2 |
| `debug_settings_access.h` | Settings-manager lookup prologue validation. | Validates the shared settings lookup implementation used by settings consumers; no independent call ABI in this header. | P2 |
| `decals.h` | EnvironmentDecalVolume manager, resource, properties, settings, and reference lifecycle. | Creates/releases manager and decal resources; initializes properties/settings; merges flagged property fields; sets opacity and world transform; balances reference ownership. | P2 |
| `descriptor_heap_size.h` | D3D12 descriptor heap sizing in renderer device setup. | Patches/validates total heap cap and per-frame cap instructions. Inputs are device tier, available descriptors, heap count, and configured cap. Instruction-boundary/control-flow contract. | P2 |
| `eadp_errors_arena.h` | EADP error reporting arena construction. | Rewrites/validates the settings-size load and vtable reference in the arena factory; reads configured arena size and uses fixed vtable. | P3 |
| `effect_surface.h` | Park-editor physics picking plus body position/rotation readers. | Takes a 16-byte physics body handle (world, index, generation); returns aligned float4 position/quaternion; shares picking validity contracts with `park_editor.h`. | P2 |
| `effects.h` | Effect manager, EffectEntity descriptor, particle/decal assets, and level compartment lifetime. | Builds/destroys 0x100-byte descriptors, creates/moves/stops/releases effect references; supplies world matrix and asset type/compartment. Stop/release enqueue work; level unload cancels/drains queued work. Plugin effect assets must be in the resident root bundle. | P2 |
| `engine.h` | Shared engine functions, vtables, TypeInfos, and globals used by many features. | Shared services include settings/model lookup, context, array/delegate/promise/string helpers, UI/audio managers, client/server/player managers, and entity/type metadata. Each consumer's own ABI and ownership contract must be recorded; this is a symbol registry, not one callable contract. | P0 |
| `engine_tweaks.h` | Enlighten GI update, mesh-cull scheduler, job wake/idle system, precise sleep, and other performance internals. | Hooks or calls frame/job/sleep functions; reads renderer settings, scheduler queues, waiter masks, worker table/TLS state, and timer abort state. Preserve thread affinity and scheduler invariants. | P2 |
| `entitlement_trace.h` | Native entitlement request path and asynchronous result chain. | Observes request construction, group/member enumeration, promises/futures, continuations, strings, vectors, and error objects. Callback/promise completion and destruction order matter, not just a return value. | P2 |
| `entity_pages.h` | Entity-page allocator and all instruction sites that assume page count/index width. | Coordinated patch set enlarges page entries/bytes and changes index shifts. Every listed instruction site and related allocator/layout must agree; partial patching is invalid. | P1 |
| `fixed_stop_entitlement.h` | Replacement/guard around CheckEntitlements and its native async result machinery. | Reads group/member arrays and eIDs; builds native result vectors/elements and strings; constructs errors; completes promises/futures and invokes callbacks; returns native-compatible ownership structures. | P2 |
| `frame_generation.h` | DLSS frame-generation option setup. | Changes/validates retain-resources flag behavior; reads mode and dynamic-mode settings. Patch must preserve the remaining options-building sequence. | P2 |
| `game_data_files.h` | Steam depot Data-folder manifest. | Launcher checks each required file path and expected size before starting the game; no game function call. | P0 |
| `game_fatal_dump.h` | Game's MiniDumpWriteDump import slot and thunk. | Identifies the game's own minidump import path; diagnostic/last-chance crash handling only. | P3 |
| `game_state_values.h` | Named skater animation/game-state values written by graph setters. | Resolves descriptor name and setter pointer; reads the named PopHeight value (and potentially other graph state values) from graph storage. | P1 |
| `game_states.h` | Skater AntDrivenComponent state pages and rate controllers. | Reads/writes float state values, resolves state storage, copies state values between contexts, and reads rate-controller output. Trainer consumer is named in header. | P2 |
| `gameplay_settings.h` | Reflected game settings sections and member records. | Resolves setting TypeInfos/vtables and member metadata; overrides/reads booleans such as fast travel, progression, activities, UI, collision, and related gameplay switches. | P1 |
| `global_offline.h` | Shared OnlineEnabled native and call sites in offline boot. | Hooks the OnlineEnabled leaf and handles specific direct/reflected call return sites, including backend, character readiness, Flow adapter, and post-spawn progression checks. | P1 |
| `graph_throttle.h` | Authored graph native call tables and delayed-event repost instructions. | Rebinds PostEventDelayedSeconds/Frames native slots and patches graph bytecode/repost sites for debug, retry, entitlement, milestone, and quest loops. Graph headers, code offsets, constants, and native call table must match. | P2 |
| `graphics_controls.h` | Filmic component, video filter render pass, GlobalPostProcessSettings. | Hooks/reads filmic component construction/destruction/property updates; resolves video filter vtables and reflected settings fields including dirty version. | P2 |
| `graphics_labels.h` | TierManager graphics settings cache. | Calls/validates tier cache rebuild and engine string assignment; primarily labels/fallback display data. | P3 |
| `grind_target.h` | Airborne rail candidate selection and skater grind-target context. | Reads context/request gate/alignment, requested target/flags, root/target/direction fields; selection still relies on native geometry and acceptance tests. | P1 |
| `guid_table_lookup.h` | Asset GUID open-addressed lookup and record retrieval. | Searches table by 16-byte key through source virtual key getter; record index is u16. Existing algorithm can loop forever on absent keys, so key-miss semantics are part of contract. | P1 |
| `hud_corner.h` | HUD DpadScoringStack and ScoringHUD widget model schemas. | Reads/writes UI model schema hashes, stack item priorities/transitions/content and score stack presentation fields. UI graph/widget logic owns visibility decisions. | P2 |
| `initfs.h` | InitFS decryption key and decryptor reference. | Reads 16-byte AES key from image and validates the instruction reference in decryptor; data plus code-reference contract. | P0 |
| `job_system.h` | Native job worker loop and spin configuration. | Reads/writes spin ticks, configured spin milliseconds, and thread count; consumers tune polling/sleep behavior. | P1 |
| `launches.h` | Local skater jump/launch trajectory setter and launch call sites. | Hooks trajectory setter and validates grind-pop/ollie/boneless call sites; trajectory parameters feed world landing queries, which use query manager gravity. | P1 |
| `level_loading.h` | Client state transition, level request, disconnect, and loading-screen selection. | Transition receives (client, next_state, previous_state); ReSkate runs pre-transition callbacks then forwards. Request/disconnect hooks are separate observational contracts; loading-screen selector returns authored info. | P0 |
| `light_tables.h` | Renderer light-ID allocator and per-kind light tables. | Reads/writes allocator free-list and issued-ID counters; hooks ID allocation/factory paths; table capacity is 64 pages × 128 entries. | P1 |
| `live_mods.h` | Layout manager, data layers, superbundle registry, install manifest/chunks, TOC and root LevelDescriptionAsset. | Registers/finds/attaches superbundles under layout lock; prepares TOC byte order; merges TOC entries and updates OnDemandBundles. Pointer ownership, file lifetime, vectors, lock, and TOC layouts are essential. | P0 |
| `load_errors.h` | Composite load-error description virtual. | Reads error-part pointer array/count and writes formatted descriptions to native text writer; must handle null/uninitialized parts safely. | P2 |
| `loading_screen.h` | RimeLoadScreen vtable and config/info TypeInfos. | Supplies native type/vtable identity used by the loading-screen selection hook; selector contract is in `level_loading.h`. | P2 |
| `local_challenge.h` | Offline challenge/activity catalog, owner/context, reflected model types and events. | Constructs/publishes ActivityData, criteria and challenge-count models; emits ActivityListUpdated; native progress hydration and event payload layouts are dependencies. | P2 |
| `local_cosmetic_catalog.h` | Retail OwnableData and inventory manager. | Supplies manager/control-block vtables and manager globals; bridges native ownable messages into the local cosmetic catalog/UI. Borrowed OwnableData pointer lifetime is significant. | P2 |
| `local_customization.h` | Starter RecipeTemplate TypeInfo. | Resolves the native recipe-template type used by local customization; caller trace needed for exact object construction/ownership. | P2 |
| `local_location_travel.h` | Offline location/access-point catalog models and notification events. | Constructs reflected location/access-point records and dispatches catalog publication events through location owner. | P2 |
| `local_music.h` | Music asset manager, MusicGraphAsset, tags, playlists, music UI manager and DTOs. | Registers/unregisters songs; looks up song by artist/title; adds songs to playlists; constructs/publishes playlist/song DTO arrays; UI and asset references have explicit ownership. | P2 |
| `local_native_settings.h` | DingoProfileSettingsManager and its option-group vector. | Reads/writes local profile setting groups (LocalPerDevice and cloud groups); manager layout and group lifetime are required. | P2 |
| `local_neighborhood.h` | Neighborhood progression records and reflected model fields. | Creates/updates neighborhood model with unlocked flag and progression hash; record TypeInfo/hash/size and field offsets are inputs to native model publication. | P2 |
| `local_object.h` | Build Kit category-result TypeInfo. | Supplies the native result type before profile/object parser fills it; data identity/layout contract. | P2 |
| `local_placements.h` | Native placed-object manager, teleport manager, client readiness and game ghosting records. | Reads/writes entity IDs, owner/group identity, forced ghost/solid flags, active/shown state and transform/render/collision lifecycle. Uses client realm context and native re-evaluation. | P1 |
| `local_player_card.h` | Appearance manager and UIPlayerInfo connection/network type metadata. | Resolves platform/network and connection-state value types for player-card UI; Steam enum value is 4. | P2 |
| `local_rip_score.h` | Collection-score progression model and reflected record fields. | Publishes/reads total Int64 score field in native progression model. | P2 |
| `localization.h` | Localized string override store and text capitalization conversion. | Inserts UTF-8 overrides by hashed ID/layer; reads existence/database state; converts text to UTF-16 and registers glyphs. String length, database readiness, locking, and allocator behavior matter. | P2 |
| `location_travel.h` | Native travel location/access-point constructors and request/prepare/attribute-request routines. | Constructs location/access records and forwards native travel requests/attribute requests; exact argument structures and async behavior need caller trace. | P1 |
| `loose_files.h` | Native VFS open/exists, directory filesystem factory, mount, global VFS. | Opens/checks paths with VFS pointer, flags and filename; creates directory filesystem and mounts it at mount point; returns native file/VFS pointers and bools. | P0 |
| `lua_natives.h` | Stock Lua 5.2 native library boundary: DLL loading, chunk parser, os/io functions. | Calls LoadLibraryExA through package.loadlib/require; parses text/bytecode according to SParser mode; standard os/io natives access process/filesystem/pipe APIs. | P2 |
| `lua_startup.h` | Native post-startup callback, Lua execution/context, settings reload. | Hooks post-startup routine, obtains Lua context, executes startup buffers, reloads settings. Lua state and startup ordering are required. | P1 |
| `media_catalog.h` | Backend services media catalog container and per-user catalogue. | Looks up/adds catalog container; user-added callback creates per-user folder/catalogue, loads entries and posts ready state. Catalogue entry layout and listener subscription matter. | P2 |
| `mesh_settings.h` | Mesh render-settings object initialization and unload cleanup. | Applies a 0x70-byte reflected settings instance at mesh +0x340; records applied flag and frees string at +0x360 on unload. | P1 |
| `mesh_variations.h` | MeshVariationDatabase registry removal during bundle unload. | Removes variation entries keyed by mesh name hash and variation hashes under registry lock; depends on mesh lifetime and 0x30-byte entry stepping. | P1 |
| `mission_progression.h` | Quest lookup, generic ECS component query, QuestState component, expression lookup. | Resolves QuestId, queries component, reads QuestState at component +0x18, evaluates terminal-state conditions and returns handles to compiled graphs. Query context/result ABI matters. | P1 |
| `mod_layers.h` | Layout.toc layer classification, layer loading and layout bootstrap. | Classifies paths, loads LCU/Patch/Data layers and uses layout-manager vtable slot +0x60. Load order and lock/manager state are part of the contract. | P0 |
| `morph_memory_pool.h` | Morph memory-pool constructor, initializer, allocate/grow routines and unwind metadata. | Validates complete code/pdata/unwind byte contracts for pool allocation/growth. Not a simple function fingerprint; exact function and unwind boundaries matter. | P1 |
| `named_settings.h` | Native settings manager typed get/set and reflection symbol/type registry. | Reads/writes bool/int/uint/float/CString settings; setter increments DdcDirtyVersion and notifies listeners. Also enumerates group/raw maps and TypeInfo field metadata under manager lock. | P1 |
| `native_audio.h` | Remote skater sound component submit/copy/reset/stop. | Captures local sound frame, merges masked primitive fields into owned frame, resets defaults and stops component/clears handle. | P2 |
| `native_cosmetics.h` | Remote skater appearance recipe and per-instance material/shader parameters. | Deep-copies recipe; publishes material map; inserts typed shader parameter values with priority; reads type/hash/layout metadata and marks material changed before publish. | P1 |
| `native_indicators.h` | HUD indicator registry, presenter, provider, widget cache and property binding. | Registers/removes providers, ticks presenter, flushes pending removals, creates widgets and binds output properties; UI model/class-reference helper contracts are shared. | P2 |
| `native_menu.h` | Native menu action descriptors, data-model record loading, widget blueprints, asset lists and texture registry. | Copies native action descriptor/invoker, loads model records, resolves widget/asset/texture types; destruction/lifetime and unloaded widget asset safety matter. | P2 |
| `native_party.h` | Native social/party UI, player lookup, weak refs, spectate/teleport, arrays and async party requests. | Reads/writes native player/member arrays; resolves EA IDs, weak player/entity refs and transforms; intercepts invite/join requests, forwards ReSkate session commands, constructs native result and invokes callback/clears handle. | P1 |
| `native_patch_support.h` | Native patch-envelope verifier and BCryptVerifySignature import/call. | Validates raw envelope/length, returns payload at +0x22c and subtracts header size; selectively accepts edited payload at inspected crypto call while preserving other crypto/provider errors. | P1 |
| `native_player_ui.h` | Native map display, player-location manager and map projection. | Creates/updates/releases player locations, appends render-message location list, projects world positions onto map and hooks map tick/destruction. | P2 |
| `native_skater.h` | Remote skater animation, entity destruction, board placement, render-pose interfaces. | Hooks animation update/destruction/render pose; reads/writes remote actor entity/board relationships; submits renderer pose data and uses native board transform/placement paths. | P1 |
| `native_throwdowns.h` | Throwdown activity state, modes, player info, network events, parameters, leaderboard and helper APIs. | Answers activity/mode/player queries; sends networked events; builds mode parameter arrays; handles participant updates, challenge starts, player lookups, native arrays/strings and callbacks. | P2 |
| `native_vfx.h` | Skater effects manager and effects component/rig/skeleton relationships. | Registers skater effects, creates impact effects, refreshes/builds effect sets, resolves joints and reads rig/skeleton effect-set arrays. | P2 |
| `native_voice.h` | Native sound manager, voice sample provider, spatializer and sound input types. | Creates/destroys sound, sets source/transform input, submits/removes PCM samples by source ID; global manager/provider/spatializer and asset vtables are required. | P2 |
| `neighborhood_unlock.h` | NeighborhoodBoundaryComponent_UpdateIsUnlocked expression binder and authored updater payload. | Rebinds expression arguments at cached and one-shot call sites; validates installed layout.toc hash and updater CAS payload identity/location. | P2 |
| `no_bail.h` | Skater bail-cause collector, physics-state selection, skeleton response, animation publishing, board placement and footplant/attach paths. | Records cause/magnitude; chooses physics state; reads/writes context flags, placement timers/blends/bones and foot-attach modes; hooks animation/skeleton flow. Several distinct contracts, not one monolithic hook. | P1 |
| `offboard_flight.h` | On-foot/off-board and ragdoll flight substates. | Reads/writes parent state, physics context, board/rig wrappers and embedded velocity; Ground/Slide add context velocity, Falling adds gravity*dt; ragdoll updates cache velocity from displacement. | P1 |
| `park_editor.h` | Build Kit dropper movement, physics picking, color override, surface ray/sphere queries and transform updates. | Queries physics/body/entity mappings, reads transforms and selection state, changes object transforms, uses scope allocator/result cleanup and query filters. Handles generation-bearing body IDs. | P2 |
| `park_rotation.h` | Authored BAM park-pack event adapter. | Hooks park manager tick, context construction and zone/park load notification; reads current manager/context. | P2 |
| `pending_updates.h` | Native transform/property snapshot pending-update queues. | Coordinated patch set changes inline page allocation while preserving owner offsets, stack frames, queue sorting, sync/async consumers, rounded iteration and sized deallocation. | P1 |
| `physics_bodies.h` | FBPhysics world body handles, body-owner mapping and ghost/solid flags. | Resolves 16-byte {world,index,generation} handles; reads owner index/entity mapping and toggles body/main-body ghost state with native notifications. | P1 |
| `physics_tuning.h` | Shared SkatePhysicsTuningAsset and per-skater physics tuning block. | Reads global asset/type/field/curve layouts; refreshes copied tuning fields and curve data in physics controller block. Asset pointer and copied block lifetime matter. | P1 |
| `physics_wake.h` | Physics entity-system per-frame wake pass and owner bitset. | Reads changed bodies, resolves owner indices, deduplicates and invokes owner update vtable; ReSkate enlarges owner pool, so bitset limit must match actual owner count. | P1 |
| `physics_world_size.h` | FBPhysicsWorld creation descriptor and all size-dependent allocations. | Patches/validates 0xa8-byte FBPhysicsWorldSize copied into job arguments before pools/arrays are sized; both native caller descriptors must be consistent. | P1 |
| `placement_adapters.h` | Build Kit placement selection/finish/pose, transform call sites, friction, point velocity, shape/body, edge/chain queries. | Supplies entity lists, selection flags, transform/pose buffers, physics handles, bounds and edge identity; hooks distinguish preview/commit and moving/material callers by return address. | P1 |
| `placement_physics.h` | FBPhysics body pose/sleep/motion type and marker save/return controller. | Reads/writes body pose and motion type, sleeps/wakes body, saves/returns marker transform, observes owner/transition state. | P1 |
| `population.h` | Traffic/pedestrian population manager by realm and TrafficSystemConfig TypeInfo. | Resolves manager pointers/vtable and config type; caller trace needed for exact operations/fields. | P2 |
| `profile.h` | Native profile loading/sending, challenge counts/indexes, bool settings, onboarding, entitlement and expression/model helpers. | Loads and caches profile data; reads/writes bool settings and onboarding state; sends profile requests; builds/tears down native strings/vectors/models and quest/challenge data. Promise/event sequencing matters. | P1 |
| `profiler_labels.h` | Engine profiler stack-sampler address-to-name labels. | Supplies RVAs for client update/frame, expression VM, job system, simulation thread and named jobs; labels profiler output only. | P3 |
| `progression_service_guard.h` | Native progression event dispatch and its call site. | Guards null progression service before dispatching two-entry event vector; preserves event context and caller cleanup. | P2 |
| `protossl.h` | DirtySDK ProtoSSL CA certificate insertion and allocator. | Calls certificate setter with PEM pointer, byte length and verify flag; must wait until DirtySDK allocator is initialized. | P1 |
| `puppet_cost.h` | Native component update passes for remote multiplayer puppets. | Reads UpdateInfo (command buffer, scaled dt, raw dt), component phase flags and vtables; hooks/defer/skip selected PreSim/PostSim components to reduce remote actor cost. | P2 |
| `ragdoll_bodies.h` | Native FBPhysics ragdoll body handles and velocity setters. | Resolves 23 rig body handles from skater core and validates generation; sets linear/angular velocity through world handles, subject to native finite-value and speed limits. | P1 |
| `reference_lists.h` | Level asset reference-list registration and unload traversal. | Filters null/empty asset references out of arrays before registration/unload; preserves count/flags and 16-byte reference semantics. | P1 |
| `remote_collision.h` | Client physics world, body allocation, capsule shapes, collision masks and shape ownership. | Allocates bodies, creates capsules from endpoints/radius, sets motion type/category/collides-with/owner/COM; 16-byte handles and world lifetime are essential. | P1 |
| `render_resource_pool.h` | Renderer resource-pool allocation/index tables and all redirected vector/metadata references. | Applies a large coordinated instruction patch to enlarge pool and update object sizes, offsets, capacities and metadata pointers. Must be atomic as a patch group. | P1 |
| `replay_activity.h` | Replay session manager/backend and playhead state. | Resolves client singleton/manager/lease/backend; reads playing flag and playhead timestamp through backend slots. | P2 |
| `replay_export.h` | Native movie encoder frame submission and Media Foundation H.264 configuration. | Reads encoder bitrate/rate mode/quality/peak/width/height/FPS; submits pixels and pitch per rendered frame while capture is active. | P2 |
| `replay_keyframes.h` | Replay camera framing keyframe segment construction and evaluation. | Rebuilds segments from session key map, builds slots from key values/time/behavior/subject, unwraps angles and evaluates Hermite spline output each frame. | P2 |
| `replay_recorder.h` | Rolling replay recorder chunk lifecycle. | Hooks end_chunk(recorder, force), reads stream/chunk bounds and writes/starts next chunk; must preserve worker-thread and allocation ownership behavior. | P2 |
| `rigid_bodies.h` | Native rigid-body force/torque, synchronizer, body/shape state arrays. | Writes force/torque/inertia ratio and reads world state rings, mass, allocation/dynamic bits, shape->body mapping and generations; each read must revalidate current world/body. | P1 |
| `route_lookahead.h` | Traffic/navigation route lookahead and endpoint/conflict projection call sites. | Validates a group of call-site contracts and return points for route exits, endpoints, reverse endpoints, conflict approach/departure and array/vector projection. | P2 |
| `runtime.h` | Client tick, menu expression, level description construction/destruction, hosted-level selection and local level request. | Hooks client tick (client, update), forwards original update; creates/destroys level descriptions and submits local level requests; checks image bytes and sublevel/message-bus layouts. | P0 |
| `score_hud.h` | ReSkate-owned scoring HUD model and trick-history rows. | Writes own UI model fields by reflected hashes, creates rows and updates score/timer/multiplier/action/sequence data; yields to native line summary rather than overwriting it. | P2 |
| `script_natives.h` | Authored graph native bindings for script error logger and SetFields. | Rebinds native table entries to logger/no-op and SetFields implementation; ownable-context resolve is a retail stub and must not be assumed functional. | P2 |
| `shader_permutations.h` | Expression shader permutation preparation on job workers. | Hooks prepare(shader, renderer, state, flags); reads permutation begin/end and prevents dereferencing empty permutation array before native dispatch. | P1 |
| `shader_registry.h` | ExpressionShaderDatabase construction/destruction and registry registration. | Constructs database from allocator/blob/lookup, registers keyed items under registry lock, unregisters on destruction; item arrays and queued job lifetime must remain valid. | P1 |
| `skater_body.h` | Skater physics body contacts and skeleton response. | Reads/writes contact records for 24 body bones, impact peaks, normals, relative velocity and tracked-point contacts; records are cleared and repopulated each physics step before skeleton response. | P1 |
| `skater_entities.h` | Skater/object blueprints, entity creation descriptors, component vtables and placement. | Builds/destroys 0x190-byte creation descriptor, creates entities with blueprint/parent bus/transform, manages source/appearance and object mesh/bounds references. | P1 |
| `skater_lead_foot.h` | Skater physics context stance flags. | Reads lead-foot bit 5 at context +0x13c4; distinguishes lead foot from switch stance. | P2 |
| `skater_model.h` | Skater component sampling globals. | Reads presence of three component globals for model sampling/diagnostics; no callable function contract in this header. | P3 |
| `skater_movement.h` | Local skater movement and offboard state machine. | Reads context velocity, parent state machine, board, context/rig and active offboard state; movement API returns live world velocity/state fields. | P1 |
| `skater_observer.h` | Skater factory/component creation and player binding. | Hooks factory(context), component creation(component, ground check), and player binding(player, entity) for lifecycle observation. | P2 |
| `skater_render.h` | Skinned render object draw packet, render manager handles and camera. | Reads packet actor matrix, bone matrices/count/root/relative/visible fields; resolves local skater render handles and guards render-object packing/locking. | P2 |
| `skater_skeleton.h` | Native render skeleton and Dem Bones skinned costume asset. | Identifies skeleton/mesh assets by TOC, bundle and asset path; uses LOD and bone mapping for pose/render tooling. Asset contract, not a game function ABI. | P2 |
| `skater_slot_override.h` | Skater loadout settings and native slot manager. | Reads/writes min/max slot settings and selection; resizes/selects native slot list and publishes list; constructor/manager/list layouts required. | P2 |
| `skater_state.h` | Live skater physics/trick/offboard state and body arrays. | Reads selected physics state, step dt, offboard/ragdoll/air flags, board and rig body arrays, velocity/spin and pending sync flags. | P1 |
| `spatial_buckets.h` | Spatial bucket/root/node/block arrays and traversal/insert/move/remove code. | Applies coordinated patch set enlarging bucket arrays and root layout while preserving queue/stack/render packet layouts; all traversal lanes and patch sites must agree. | P1 |
| `spatial_cull_ids.h` | SpatialQueryManager cull-ID free-list creation and size. | Reads SpatialQueryManagerSettings.InitialCullIdListSize and sizes u32 next-ID/u64 entry arrays; insert path assumes list has a free ID. | P1 |
| `startup_interventions.h` | Lua VM frame clearing and exit path instructions. | Captures/patches byte-store frame clearing and exit thunk/stack state; depends on VM frame layout and exact instruction boundary. | P1 |
| `streaming_reads.h` | End of scattered I/O read and staged-copy destination list. | Hook receives scheduler/request; reads staged flags and place count/offset/size/stride/destination; zeroes offending copy size before forwarding to prevent overrun of 64-KiB staging buffer. | P1 |
| `texture_handles.h` | Renderer texture-handle pool, generation redirects, default texture and view cache. | Resolves {u16 id,u8 weak} handles; reads texture/redirect entries and guards null/dead generation paths before material view-cache insertion. | P1 |
| `texture_streaming.h` | TextureStreamingManager 16-bit handles, slot arrays and all code references to manager allocation/offsets. | Enlarges manager allocation and moves slot/float/list storage; coordinated instruction-site sweep updates every offset/count. | P1 |
| `throwdown_debug_text.h` | Throwdown graph debug text/player-name/screen-size fast thunks and repost. | Supplies native fast-function pointers and patches ClientSkateThrowdown.OnDebugPrint repost slot; graph operands and one-frame delay must remain valid. | P3 |
| `traffic_vehicles.h` | Moving ECS entity hierarchy update and component-query interaction. | Hooks/observes hierarchy update and uses local/world transform TypeInfos; component-query prefix is affected by mission progression Detours hook. | P2 |
| `trick_rewrite.h` | Native trick recorder and localized trick ticker labels. | Hooks recorder(recorder, packet), reads 0x68-byte packet ID/valid fields and resolves localized key into caller buffer; preserve history copy and success result. | P2 |
| `ui_model.h` | Native UI model write and publish path. | Writes one typed value to model handle under model lock and publishes through model manager; value bytes/type/flag and handle lifetime matter. | P2 |
| `ui_pointer.h` | UI per-frame pointer hit-test and mouse cursor mode. | Hooks pointer-still/moved and cursor enable/force functions; reads/writes mouse force count, deferred/requested/applied cursor state and fills UI hit list. | P2 |
| `ui_textures.h` | Installed game's UI/scoring texture assets. | Provides TOC/bundle/asset-name mappings and image regions/slices for overlay widgets; asset lookup contract rather than callable function. | P2 |
| `ui_values.h` | UI model symbolic aliases and typed model registry. | Inserts aliases and reads model/alias table offsets; callers hold ModelWriteLock. | P2 |
| `unload_guard.h` | Level-unload object table walk and asset +0xc8 read. | Rewrites loop to skip null asset references while preserving loop step and remaining instructions; patch/control-flow contract. | P1 |
| `visual_environment.h` | VisualEnvironment entity and owner tagged-reference data. | Resolves entity vtable and TypeInfos used by native environment component/reference dispatch. | P2 |
| `world_controls.h` | Fog/sky/wind environment component sites, vtables and TypeInfos. | Hooks/patches 15 environment control sites and reads component/type identity arrays; exact property layouts are in `atmosphere.h`. | P2 |
| `world_layers.h` | SubWorldReference constructor/destructor/event handler and event clone. | Tracks subworld reference lifecycle; clones 16-byte event into owned arena rather than retaining caller pointer. Vtables and event ownership are critical. | P1 |
| `world_model.h` | Level registry and server sublevel manager globals. | Reads level metadata array and sublevel manager pointers used during world/sublevel operations. | P0 |
| `world_pool_usage.h` | Entity pages, render instance pool and spatial bucket usage counters. | Reads allocator snapshots/counters to determine actual peak pool use and whether native capacities are sufficient. | P1 |
| `world_props.h` | ObjectBlueprint/static-model entity creation and transform updates for ReSkate props. | Resolves object blueprint and static-model entity vtables; creates client-only prop entities and moves them through entity transform setter. | P2 |

## Dependency tiers and recovery order

### First-pass P0 set

1. `game_data_files`: launcher must verify the executable's supporting Data payload.
2. `initfs`: decryption key/reference required for game asset access.
3. `runtime`: client tick, level description, local request and image checks.
4. `level_loading`: transition hook and callback ordering.
5. `world_model`: level registry and sublevel manager.
6. `mod_layers`, `live_mods`, `loose_files`: layer discovery, VFS overlay, superbundle registration and TOC integration.

This set is the initial map-transition/mod-content spine. A match for only the transition function is insufficient if level request descriptors, VFS, layer loading, superbundle registration or sublevel globals no longer satisfy their contracts.

### First-pass P1 set

Prioritize the world/asset lifecycle around the P0 spine:
- `world_layers`, `streaming_reads`, `reference_lists`, `guid_table_lookup`, `mesh_settings`, `mesh_variations`, `shader_registry`, `shader_permutations`, `unload_guard`.
- `entity_pages`, `pending_updates`, `spatial_buckets`, `spatial_cull_ids`, `physics_world_size`, `physics_wake`, `physics_bodies`.
- `skater_entities`, `skater_state`, `skater_body`, `physics_tuning`, `offboard_flight`, `no_bail`.
- `native_party`, `native_skater`, `remote_collision`, `ragdoll_bodies`.

### Matcher contract requirements

For every callable candidate, store at least:
1. Original header/symbol and all call/hook sites.
2. Native ABI as observed by ReSkate: argument count/order, integer vs floating registers, stack arguments, pointer/value semantics, return value.
3. Inputs ReSkate supplies, including object identity, realm/thread, flags, sizes, and any constructed native structures.
4. Outputs ReSkate consumes, including out-parameters, mutations, callbacks, promises/futures and ownership transfer.
5. Side effects required for equivalence: reference counts, locks, event dispatch, callbacks, queue insertion, state transitions, or synchronization.
6. Object layouts and lifetime assumptions associated with the call.
7. Strict candidate constraints first (same ABI, same argument roles, close instructions); relaxed candidates may change ABI/layout only when an adapter can be described.
8. Confidence must be separated into **behavior match**, **ABI compatibility**, and **runtime/lifetime compatibility**. A behavioral match is not automatically a safe function pointer.

For non-call entries, store contract-specific requirements instead:
- **Patch:** complete instruction bytes, instruction boundaries, incoming branches, expected surrounding data/control flow, and all coupled sites.
- **Layout/global/vtable:** object/global identity, field offsets/types, ownership, realm/thread, lifetime, and producer/consumer evidence.
- **Asset/data:** hashes, names, schema sizes, TOC/bundle location, and load/unload scope.

## Remaining verification ledger

This document closes the inventory gap: every one of the 137 build-specific headers has an entry. It does **not** close all caller-tracing gaps. Before implementing the matcher, promote each entry to one of:
- **Verified contract:** ReSkate consumer and game-side input/output/side effects traced.
- **Header-described contract:** useful ABI/layout semantics are documented in the header, but the ReSkate consumer has not been independently checked.
- **Data/patch contract:** not a callable function; use the appropriate non-call matching method.
- **Unresolved:** insufficient evidence; do not infer an ABI from the RVA or prefix alone.

In particular, do not collapse a header containing several independent symbols into one contract. `level_loading.h`, `native_party.h`, `profile.h`, `engine.h`, `no_bail.h`, `park_editor.h`, and `native_throwdowns.h` each contain multiple contracts with different priorities and/or call semantics.
