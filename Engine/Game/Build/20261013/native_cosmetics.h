#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include "native_cosmetics_rvas.h"

// Remote skater and skateboard cosmetics.
namespace dingosdk::game::build::v20261013::native_cosmetics {
// Customization component that owns the recipe at +0x000.
// Deep-copies a recipe value into a component (component, recipe).
inline constexpr std::array<unsigned char, 24> recipe_copy_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// Publishes this AppearanceItem's per-instance named material map to its render meshes.
// (item, map_index), with map_index 0 for addMaterial("Top_mat").
inline constexpr std::array<unsigned char, 24> publish_materials_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// Typed per-instance shader setter reached by MaterialInstance:addEsVector.
// (material, ParamDbKey*, NativeValue*, priority) -> success. Inserts with the
// engine allocator and preserves the existing priority rules.
inline constexpr std::array<unsigned char, 24> set_shader_parameter_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// Initialized ColorRgb type descriptor; validate its type hash, size and index.
// A material instance's own shader parameters (an AppearanceItem's named material): a hash map at
// +0x000 of buckets, +0x000 its bucket count, +0x000 how many it holds. A node holds its ParamDbKey
// at +0 (identity, size, type index and count words, then the key's 64-bit hash at +8), the value
// at +0x00, the value's type object at +0x00, flags at +0x00 (bit 0 set on every live one) and the
// next node at +0x00. A value written there shows once the material is marked changed at +0x000 and
// its item publishes its materials (publish_materials). Read from the running game's board, 2026-10-22.
inline constexpr std::size_t material_parameters = 0x000;
inline constexpr std::size_t material_parameter_buckets = 0x000;
inline constexpr std::size_t material_parameter_count = 0x000;
inline constexpr std::size_t material_changed = 0x000;
inline constexpr std::size_t parameter_value = 0x00;
inline constexpr std::size_t parameter_type = 0x00;
inline constexpr std::size_t parameter_flags = 0x00;
inline constexpr std::size_t parameter_next = 0x00;
// The type hash (the first word of the TypeInfo a type object points to; its size is the word at
// +6) of each kind of value the board's materials hold: ColorRgb, Float32, an integer (values like
// 13 on the deck, which pick how it is drawn) and a boolean (a switch the shader was compiled with).
inline constexpr std::uint32_t color_rgb_type_hash = 0x00000000;
inline constexpr std::uint32_t float_type_hash = 0x00000000;
inline constexpr std::uint32_t integer_type_hash = 0x00000000;
inline constexpr std::uint32_t boolean_type_hash = 0x00000000;
// Finds a named entry in a loaded table by the FNV-1a hash of its name:
// (out, name, table, flag, context) -> out, the fifth passed on the stack.
// The skater loader's preset lookups reach it through a Lua binding that
// passes a nil table on when a preset's bundle was not found, and the function
// reads the table without checking it.
inline constexpr std::array<unsigned char, 24> named_lookup_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// The skater scripts' item:addEsShaderPreset(material, preset): a Lua function (state) -> count
// of results. Its third argument is a reference to the loaded preset, which it uses without
// checking that the preset is there: an item whose preset has gone (or never loaded) by the
// time its script runs crashes the game in the material's construction.
inline constexpr std::array<unsigned char, 16> add_shader_preset_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// The two calls it makes to read that argument: (state, argument number) -> the reference, and
// (reference, 0) -> the preset it holds, or null.
inline constexpr std::array<unsigned char, 8> script_preset_reference_prefix{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// An AppearanceShaderExpressionPreset as loaded (its EBX layout): ParamInfo, the array of its
// AppearanceExpressionParamInfo at +0x00, 40 bytes each: the value's boxed {TypeInfo*, data*} at
// +0, its Parameter at +0x00, its ParamKey at +0x00. Applying a preset to a material (0x000000000,
// under 0x000000000) type-checks every entry whose data is set, reading the TypeInfo unchecked.
inline constexpr std::size_t preset_parameters = 0x00;
inline constexpr std::size_t preset_parameter_size = 40;
inline constexpr std::size_t preset_parameter_key = 0x00;
// What item:addEsShaderPreset does with the item, the material and the preset, besides keeping the
// preset with the item (2026-10-21, for items worn on top: layer_materials.h):
// - the Lua argument as a userdata of a type: (state, argument number, type name) -> its block, which
//   holds the native AppearanceItem; an AppearanceMaterial argument is the material itself;
// - an AppearanceItem's material by name (AppearanceItem:addMaterial): (item, 0, has name, name hash,
//   name, variant 0x00) -> the material, made when the item has none of that name. The name is kept:
//   it must outlive the item;
// - then on the material: (material, the preset's name), (material, item), (material, the preset's
//   Lua reference).
inline constexpr std::array<unsigned char, 16> script_userdata_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
inline constexpr std::array<unsigned char, 24> item_material_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
inline constexpr std::array<unsigned char, 16> material_preset_name_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
inline constexpr std::array<unsigned char, 16> material_item_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
inline constexpr std::array<unsigned char, 9> material_preset_prefix{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
inline constexpr std::array<unsigned char, 17> referenced_preset_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
}
