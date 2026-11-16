#pragma once
#include <array>
#include <cstdint>
#include "placement_adapters_rvas.h"

// The hooks that let a claimed placement be selected and edited while it moves, and be skated as a
// moving surface (Extension/Objects/placement_adapters.cpp). Functions are checked by their first 16
// bytes; every call site in transform_calls, moving_calls and material_calls is checked to be a direct
// call (E8 rel32) to the function it is listed for, since the hooks tell those callers apart by return
// address (site + 5). The addresses were found by the PR #356 author against this build.
namespace dingosdk::game::build::v20261013::placement_adapters {
// The dropper's selection: select(controller, const entity list*, bool quiet) starts one; finish
// (controller, const entity list*, bool keep, bool cancel, bool force) ends it, committing unless
// cancel. pose(float out[12], const query*) -> out reads an entity's transform (scale, rotation,
// position), as the selection does to start from an object's pose.
inline constexpr std::array<unsigned char, 16> select_prefix{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
inline constexpr std::array<unsigned char, 16> finish_prefix{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
inline constexpr std::array<unsigned char, 16> pose_prefix{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
// The dropper's own calls of the transform setter (park_editor.h movement_contracts[2]), all in the
// selection code 0x7c1cc8-0x7d161c: a pose set from one of them is a preview or commit of an edit.
// Callers of motion_type (placement_physics.h) in the skater's surface code that treat a dynamic body
// as something not to stand or grind on: they are shown a claimed surface as keyframed (moving_calls)
// or, for the material lookups, fixed (material_calls).
// friction(grind, float, float, float): a grind's friction step, reading the skater context's velocity.
// point_velocity(const Handle* body, float out[4], const float point[4]) -> out: a body's velocity at
// a world point. shape_body(const Handle* shape, Handle* out) -> out: the body a shape belongs to.
// edges(cache): gathers authored grind edges around a grind context (see query_half). edge_query
// (registry, const float bounds[8], out) -> bool: gathers edges overlapping a box, bounds being its min
// (floats 0-2) and max (floats 4-6). chain(const u64 identity[2], {const Edge*, u32 count}* out) ->
// bool: the whole chain an edge belongs to.
inline constexpr std::array<unsigned char, 16> friction_prefix{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
inline constexpr std::array<unsigned char, 16> point_velocity_prefix{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
inline constexpr std::array<unsigned char, 16> shape_body_prefix{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
inline constexpr std::array<unsigned char, 16> edges_prefix{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
inline constexpr std::array<unsigned char, 16> edge_query_prefix{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
inline constexpr std::array<unsigned char, 16> chain_prefix{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
// The selection controller's set of selected entities, a hash set: bucket array, bucket count, size,
// and each node's next pointer (a node starts with its u64 entity).
inline constexpr std::uintptr_t selection_buckets = 0x000, selection_count = 0x000, selection_size = 0x000, selection_next = 0x00;
// local_placements.h object_root writes a root {entity, flags} where flags (+8) carries
// root_dynamic_restriction (0x000) for a placed object; while it is set the game keeps the object's
// body fixed. root_hook clears it in the returned copy (never in the entity) for claimed objects only.
// What the bit is called in the game is not known; its effect is the PR author's finding.
inline constexpr std::uint32_t root_dynamic_restriction = 0x000;
// An edge list is a u32 count, then at edge_data at most 100 edges of 0x00 bytes. A grind context
// (the edges cache's first pointer) has its flags (context_flags; disabled_query set means it gathers
// nothing) and position; the gather box is that position +/- query_half, a global vec3 that is
// (1.2, 1.2, 1.2) in this build. shape_generations is a world state's byte array of shape generations.
inline constexpr std::uintptr_t edge_data = 0x00, shape_generations = 0x000, context_flags = 0x0000, context_position = 0x00;
inline constexpr std::uint32_t disabled_query = 0x400000;
// A grind's skater context pointer (grind + grind_context).
// A skater's board (core + skater_board, skater_movement.h) has its physics object (board_physics,
// whose vtable is skater_state.h board_physics_vtable) and that its parts: a count of board_part_count
// (u32), the owner back pointer and the deck part's position.
inline constexpr std::uintptr_t board_physics = 0x00, board_parts = 0x00, parts_owner = 0x00, parts_position = 0x00;
inline constexpr std::uint32_t board_part_count = 9;
// The support rays cast down through the deck to find a claimed surface under the board: from
// board_ray_above above the deck's position to board_ray_below under it, at these offsets across it.
inline constexpr float board_ray_above = .4f, board_ray_below = .6f;
inline constexpr std::array<float, 7> board_ray_offsets{0.f, -.08f, .08f, -.16f, .16f, -.24f, .24f};
} // namespace dingosdk::game::build::v20261013::placement_adapters
