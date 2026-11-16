#pragma once
#include <array>
#include <cstdint>
#include "native_skater_rvas.h"
// Remote multiplayer skater and skateboard actors.
namespace dingosdk::game::build::v20261013::native_skater {
// Skater animation component update (component, update). Hooked to apply remote poses.
inline constexpr std::array<unsigned char, 18> animation_update_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00};
// Expected code of engine::board_render_pose.
inline constexpr std::array<unsigned char, 15> board_render_pose_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// The evaluated render interface is embedded at this offset in its animation holder.
// Both the local and menu board allocate their animation holder in this entity block.
// Measured on two live actors; the inverse lookup must still validate every ownership link.
// Render pose interface vtable; slot +0x00 is engine::board_render_pose.
// Hands a skeleton's render data to the renderer (slot, render_data): its caller (0x2c20d99) calls it
// right after engine::board_render_pose, with the same render data, and the bones are copied for the
// render inside it (2026-10-25, a guard page on the local skater's bone buffer). A thunk into the
// renderer's interface (+0x00).
inline constexpr std::array<unsigned char, 26> submit_render_pose_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// Entity helper verified together with the creation functions; not called directly.
inline constexpr std::array<unsigned char, 17> entity_helper_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00};

// Skateboard placement. set_blueprint_transform(transform, matrix) updates the board
// blueprint's world transform (blueprint_transform_vtable); place_board(entity, matrix) is the
// native source setter that places the entity and then updates the rig input (place_board_rig_call).
inline constexpr std::array<unsigned char, 16> set_blueprint_transform_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
inline constexpr std::array<unsigned char, 25> place_board_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
inline constexpr std::array<unsigned char, 18> place_board_rig_call_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00};
// ClientSkateboardEntity's world-transform event: updates the blueprint transform before
// notifying entity placement. Reference only.
// Placement callback that publishes inverse(parentWorld) * entityWorld to the renderer. Reference only.

// Skateboard resources.
// Enables a board animation holder's render resources (holder, 1); the same call native board
// startup (board_startup) makes. Board holder vtable slot +0x00.
inline constexpr std::array<unsigned char, 16> enable_board_resources_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// Native board startup. Reference only.
// engine::board_holder_vtable slots +0x00 and +0x00 (resource lifecycle).
// Checks board component +0x00 before creating skateboard physics. Reference only.
// Initializes every entity of a blueprint creation list, as ClientSkateboardSource does.
inline constexpr std::array<unsigned char, 13> initialize_creation_list_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// Allocator dispatch that frees a creation list page (list, page, size).
inline constexpr std::array<unsigned char, 7> free_creation_page_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00};

// Pose layout (native_pose_layout.h). Reference only.
// Post-construction callback that caches the skater component at entity+0x000.
// Follows the skater component's +0x00 backlink to its separate ClientSkateboardEntity.
// Returns the board entity's animation holder (+0x00).
// Resolves the current output pose with a signed table index; the live 395-bone skater uses -1,
// selecting table word 18.
inline constexpr std::uintptr_t render_animation_interface_offset = 0x00;
inline constexpr std::uintptr_t board_embedded_holder_offset = 0x000;

}
