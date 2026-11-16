#pragma once
#include <array>
#include <cstdint>
#include "skater_entities_rvas.h"

// Skater and skateboard entity functions and types shared by the AI skaters,
// the remote multiplayer actors and the local client source spawn.
namespace dingosdk::game::build::v20260929::skater_entities {
// Blueprint asset TypeInfo. Named character blueprints derive from it.
// ObjectBlueprint ClassInfo: a world object's blueprint (props, checkpoints), a different branch
// of the type hierarchy from the skater Blueprint above.
// A world object blueprint's mesh and its bounds, read in a running game on 2026-10-21 (a trash bag
// 0.66 x 0.58 x 0.62 m, a park bench 3.08 x 0.86 x 0.71 m, a traffic cone 0.32 x 0.60 x 0.32 m,
// each with its origin at its base): the blueprint's object data right after it (blueprint + 0x00,
// also its +0x00), whose mesh asset is at +0x00; the mesh asset's loaded mesh at +0x00, and that
// mesh's box at +0x00 -> {min float[4], max float[4]}, in the object's own frame. A blueprint
// without a mesh there (a widget, a logic object) shows nothing when it is made.
// Secondary vtables of the skater animation component (engine::skater_component_vtable) at +0x00 and +0x00.
// ClientSkaterSource::applyAppearance. It enables the customization component and requests
// appearance mode 1 (default/session) when the source data has no recipe. Reference only.

// Entity creation. The 0x000-byte creation descriptor is constructed in place by
// descriptor_init(descriptor, id, parent bus, transform); descriptor_destroy(descriptor + 0x00)
// releases its internal list state. create_entity(result, descriptor, blueprint, 0, 0) returns
// {entity, retained sublevel, ...}.
inline constexpr std::array<unsigned char, 12> descriptor_init_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
inline constexpr std::array<unsigned char, 17> descriptor_destroy_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00};
inline constexpr std::array<unsigned char, 14> create_entity_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// Destroys an entity (entity, owner). Hooked so SDK-created actors are forgotten first.
inline constexpr std::array<unsigned char, 14> destroy_entity_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// Releases one reference on an entity bus or a retained sublevel.
inline constexpr std::array<unsigned char, 16> release_reference_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

// Placement. place_entity is the skater/board entity vtable slot +0x000 transform setter.
// initialize_placement(entity, transform, 0, 1) runs the entity's +0x00/+0x00 lifecycle
// callbacks and binds/initializes its owned components.
inline constexpr std::array<unsigned char, 12> place_entity_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// Generic SpatialEntity setter used by place_entity. Copies both matrix halves
// into the owned transform collection, then notifies components via +0x000.
inline constexpr std::array<unsigned char, 22> move_spatial_entity_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
inline constexpr std::array<unsigned char, 15> initialize_placement_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

// Returns an entity's customization (appearance) component.
inline constexpr std::array<unsigned char, 12> customization_component_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// Expected code of engine::set_customization_flag.
inline constexpr std::array<unsigned char, 8> set_customization_flag_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

// Skater component animation physics flag setters (component, value) for +0x00 and +0x00.
inline constexpr std::array<unsigned char, 13> physics_c7_setter_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
inline constexpr std::uintptr_t object_data_offset = 0x00;            // blueprint
inline constexpr std::uintptr_t object_data_mesh_offset = 0x00;       // object data -> mesh asset
inline constexpr std::uintptr_t mesh_asset_loaded_offset = 0x00;      // mesh asset -> loaded mesh
inline constexpr std::uintptr_t loaded_mesh_bounds_offset = 0x00;     // loaded mesh -> {min, max}

}
