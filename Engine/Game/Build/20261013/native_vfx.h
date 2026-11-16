#pragma once
#include <array>
#include <cstdint>
#include "native_vfx_rvas.h"

// Skater effects (sparks, dust, landing puffs, costume and board effects): the game's skater
// effects manager.
namespace dingosdk::game::build::v20261013::native_vfx {
struct Function {
    std::uintptr_t rva;
    std::array<unsigned char, 16> prefix;
};
// (manager, key, component): a skater's effects component registers with the manager.
inline constexpr Function register_skater{
    0x4874800, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};
// (manager, key, impact): the effect of one contact with the world, chosen by the surface's
// material and the speed. Each call is one effect that ends by itself. Reads the calling
// thread's world.
inline constexpr Function impact{
    0x486cda0, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};
// (manager, key): the skater's effects are stopped and built again from its settings at the
// manager's next update. Takes the manager's own lock.
inline constexpr Function refresh{
    0x4874a20, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};
// (manager, uint64 *effects_entity, int id, settings, char flag, char queued): builds a skater's
// effects from its settings, a pointer to an array of effect sets (pointers, bit 2 a flag;
// the count sits four bytes before the first). Called for each refreshed skater from the
// manager's update.
inline constexpr Function build_effects{
    0x48700c0, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};
// Manager: the root of its map of registered skaters; a node's id and effects component.
inline constexpr std::size_t manager_skaters_root = 0x00, skater_node_id = 0x00, skater_node_component = 0x00;
// An effects component: what holds its entity.
inline constexpr std::size_t component_owner = 0x00;
// (rig, joint id, 1): the joint's index in the rig's skeleton, or -1. Reads no pose.
inline constexpr Function joint_index{
    0x12e08b0, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};
// joint_index looks the joint up in the skeleton's map (EASTL: a node's right child at +0, its
// left at +8, its joint id at +0x00; the root at skeleton +0x00), with no bound on the walk
// (0x4897f40).
inline constexpr std::size_t skeleton_joints_root = 0x00, joint_node_right = 0, joint_node_left = 8, joint_node_id = 0x00;
// An effects component's rig: component -> its rig's owner -> the rig; the rig's skeleton. An
// effect set's effects (an array of pointers), and the joint each sits on (-1: none).
inline constexpr std::size_t component_rig_owner = 0x00, rig_owner_rig = 0x00, rig_skeleton = 0x000, set_items = 0x00,
                             item_joint = 0x00;
// How an effect set is started, and the value for one that is always on.
inline constexpr std::size_t set_trigger = 0x00;
inline constexpr std::int32_t trigger_always = 3;
// (out): the calling thread's world, or null.
inline constexpr Function thread_world{
    0x18b6ae0, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};
// Offsets into the world, each stored as a 32-bit value: the effects manager, and the object
// that holds the level's material grid.
inline constexpr std::size_t manager_ready = 0x000;        // byte: the manager is in use
inline constexpr std::size_t materials_ready = 0x00;       // byte: the grid object is in use
inline constexpr std::size_t materials_grid = 0x00;        // the grid
inline constexpr std::size_t grid_data = 8;                // its data (bit 2 is a flag)
inline constexpr std::size_t grid_index_table = 0x00;      // data: pointer to the index table
// The effects component of a skater entity.
inline constexpr std::size_t component_size = 0x00;
// The impact: position, velocity and normal as four floats each, then the material.
inline constexpr std::size_t impact_size = 0x00, impact_velocity = 0x00, impact_normal = 0x00, impact_material = 0x00;
inline constexpr std::uint32_t material_valid = 0x00;
inline constexpr unsigned material_shift = 6;
}
