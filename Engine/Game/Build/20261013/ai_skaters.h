#pragma once
#include <array>
#include <cstdint>
#include "ai_skaters_rvas.h"
// AI skaters spawned from the player blueprint.
namespace dingosdk::game::build::v20261013::ai_skaters {
// Entity bus: create_bus(owner, parent, 0, 0, 0, 0) creates a child bus and acquire_bus adds
// a reference (released with skater_entities::release_reference).
inline constexpr std::array<unsigned char, 19> create_bus_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00};
inline constexpr std::array<unsigned char, 13> acquire_bus_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// Writes the source data+0x00 flag into the entity's +0x000 property binding.
inline constexpr std::array<unsigned char, 15> set_property_binding_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// Skater entity vtable slot +0x000. Reads the +0x000 binding unchecked once the skater's physics exists.
// The +0x000 load inside binding_reader.
inline constexpr std::array<unsigned char, 7> binding_read_site_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// Skater component physics setter fed from source data+0x00 after placement.
inline constexpr std::array<unsigned char, 17> physics_source_setter_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00};
// Expected code of skater_entities::physics_cb_setter.
inline constexpr std::array<unsigned char, 7> physics_cb_setter_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// Native named-asset lookup that searches a domain and its parents. Reference only.

// EATBrainTree component (collection slot 12). brain_tree_factory creates the bt_default
// instance (+0x00) through holder +0x00 (brain_tree_holder_vtable); brain_tree_update (vtable
// +0x000) ticks the tree only while run flag +0x00 is set.
// Reference only.

// SkaterAIComponent (collection slot 14, created by skater_ai_factory, 0x00 bytes). Its update
// (vtable +0x000) creates the 0x0000-byte AI skater controller at +0x00 (ai_controller_create,
// bound to the skater's physics body) and runs it (ai_controller_run) only while +0x00 is set.
inline constexpr std::array<unsigned char, 18> skater_ai_update_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00};
// Reference only.
// Reference only.
// Reference only.
}
