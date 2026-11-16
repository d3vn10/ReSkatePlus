#pragma once
#include "Engine/Game/Build/fingerprint.h"
#include "engine.h"
#include "remote_collision.h"
#include <array>
#include <cstdint>
#include "park_editor_rvas.h"

namespace dingosdk::game::build::v20260929::park_editor {
// The calls native dropper movement (0x7c63f0) uses to update an existing
// object: physics component query, physics body setter and transform setter.

// Source argument for the native transform setter's change notification.

// Native Build Kit color override scope: query (constructor), finish (destructor)
// and the settings manager lookup.


// Picking: hit body, body valid, body entity, entity mapping manager, mapped
// entity lookup and Build Kit component query.


// Surface probe: physics world, camera ray query, scope allocator and result
// release. The ray query (0x26afdc0) and the dropper's sphere query (0x26c0fd0)
// share the 0x2664c00 implementation and collision filter; 0x7cc4f0 and
// 0x175fed0 clean up the result the same way.

// Query filters for that ray request (request + 0x00). The wrapper passes it through
// 0x2664c00 to 0x2627bd0's query-category mask, and a candidate shape is tested only
// when its categories AND this mask are nonzero. camera is the filter the native
// camera ray sends. terrain is the game's serialized physics category query preset
// "Terrain", category bit 0 alone. The serialized presets do not show which shapes
// carry that bit: a terrain hit is not proof of terrain, static geometry or an
// actor-free result, and the Static preset lacks bit 0, so some ramps and props
// may not be tested at all.
inline constexpr std::uint32_t camera_query_filter = 0x00000000;
inline constexpr std::uint32_t terrain_query_filter = 0x00000000;
// A diagnostic filter with every bit set, so the AND gate tests every shape whose
// categories are nonzero, actors' shapes included. The game's capsule wrapper starts
// its own mask fields at FFFFFFFF. A shape with no category is still never tested.
inline constexpr std::uint32_t all_query_filter = 0x00000000;
// The categories whose definition names say character, ragdoll, character proxy,
// skateboard or skater, plus bit 31, which only the SkateboardCollisionHelper shape
// template carries. These are definitions and templates, not live assignments: a
// shape with none of these bits is not shown to be free of an actor. Bits 6 and 12
// of the Character template are left out on purpose, because the Terrain and
// DynamicGameplay templates carry them too and would be refused as actors.
inline constexpr unsigned category_skateboard_collision_helper = 31;
inline constexpr std::uint32_t named_actor_categories =
    (1u << remote_collision::category_character) | (1u << remote_collision::category_ragdoll) |
    (1u << remote_collision::category_remote_character_proxy) | (1u << remote_collision::category_skateboard) |
    (1u << remote_collision::category_local_skater) | (1u << remote_collision::category_character_proxy) |
    (1u << remote_collision::category_remote_skater) | (1u << category_skateboard_collision_helper);
static_assert(named_actor_categories == 0x00000000);
// Only a filter that shares no bit with them can back a claim that no returned hit
// carried one. Camera and terrain qualify; all does not, and is refused for it.
static_assert((camera_query_filter & named_actor_categories) == 0);
static_assert((terrain_query_filter & named_actor_categories) == 0);
static_assert((all_query_filter & named_actor_categories) != 0);
}
