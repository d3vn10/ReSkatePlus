#pragma once
#include "Engine/Game/Build/fingerprint.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include "skater_render_rvas.h"

namespace dingosdk::game::build::v20261013::skater_render {
// Supported SHA-256 fbce74d5e28ef525dbba2cb4adbebc13405bdbd88f31bc940bca45e4ae88b8f9.
// The local skater as the renderer draws it: the skinning matrices of its mesh and the camera of
// the same picture (Extension/Skater/xray_render.h). Read 2026-10-06; the draw packet,
// the render view and the handle chain were first used by gBGYo's ReSkate fork
// (github.com/gBGYo/ReSkate).
//
// A skinned render object hands its draw packet out through 0x48db8e0(object, packet), packing
// the bones first when they changed (0x000000000): each bone's skinning matrix (vertex in the
// mesh's model space to the world, the skeleton's inverse bind already in it) placed by the
// object's world matrix, three rows of four floats, transposed. The placement leaves the world
// position's whole metres out: the packet's root (float[4]) adds them back, and a relative packet
// is in its actor's frame (the packet's first matrix) besides. Bone 0 keeps its own position.


// Which render object is the local skater's: the skater entity keeps a render handle for riding
// and one for walking (entity+0x000, 8 bytes apart; each mode its own render object). A handle
// names a record in a manager's paged pool (handle - 2 is its index; index + 16 has its highest
// bit at page + 4, the pages from pool+8 on), checked in each manager's own lookup: the first
// record keeps the second manager's handle at +0x00, the second the render index + 2 at +0x00.
inline constexpr std::size_t render_modes = 2;
inline constexpr std::uint32_t first_handle = 2;
struct RenderManager {
    std::uintptr_t global; // the manager's pointer
    std::uintptr_t vtable;
    std::size_t record_size;
    std::uintptr_t handle_offset; // in its record: the next handle
};
inline constexpr std::array<RenderManager, 2> handle_chain{{
    {0x71b78f8, 0x6671138, 0x00, 0x00},
    {0x77ef158, 0x666d4c0, 0x00, 0x00},
}};
// The managers' own lookups: the first's (0x000000000, its vtable slot 65, passes this+0x00)
// finds its 0x00-byte records in the pool at +0x00+0x00; the second's (0x000000000, reached from a
// secondary vtable) its 0x00-byte ones. That the second's pool is at +0x00 as well, gBGYo measured.



// The camera of each rendered view: 0x3f8b2b0(blackboard, current view, previous view, jitter)
// copies the 320-byte view (0x000000000) for the shaders. In it the camera's world matrix at
// +0x00 (rows right, up, back, position; their fourth lanes not the matrix's) and the vertical
// field of view in radians at +0x000. The main picture's view has kind 0 (uint32), 0x00 bytes
// before it.


inline constexpr std::uintptr_t render_index_offset = 0x000; // render object, uint32
inline constexpr std::uintptr_t render_object_lock_offset = 0x000; // render object, uint32: set while packing
inline constexpr std::uintptr_t packet_actor_offset = 0x00;     // float[16], copied from object+0x00
inline constexpr std::uintptr_t packet_bones_offset = 0x00;     // pointer to float[12] per bone (object+0x00)
inline constexpr std::uintptr_t packet_bone_count_offset = 0x00; // uint32 (object+0x00)
inline constexpr std::uintptr_t packet_root_offset = 0x00;      // pointer to float[4] (object+0x00)
inline constexpr std::uintptr_t packet_relative_offset = 0x00;  // byte (object+0x00)
inline constexpr std::uintptr_t packet_visible_offset = 0x00;   // byte (object+0x00)
inline constexpr std::uintptr_t entity_render_handles_offset = 0x000; // skater entity, uint32 x 2
inline constexpr std::uintptr_t entity_render_handle_stride = 8;
inline constexpr std::uintptr_t manager_pool_offset = 0x00;
inline constexpr std::uintptr_t view_camera_offset = 0x00;  // float[16]
inline constexpr std::uintptr_t view_fov_offset = 0x000;    // float, radians
inline constexpr std::uintptr_t view_kind_back = 0x00;      // uint32, before the view

}
