#pragma once
#include "Engine/Game/Build/fingerprint.h"
#include <array>
#include <cstdint>
#include "light_tables_rvas.h"

// The light renderer (Extension/World/light_table_guard.cpp). Each kind of light has a table of 64
// pages of 128 and an ID allocator at manager+0x00; nothing checks the IDs against the table.
namespace dingosdk::game::build::v20260929::light_tables {
// The allocator: the free list head at +0x00 (low 32 bits 0x00000000 when empty), fresh IDs given at +0x00.


// factory(creator, creation info): the entity, or null. The six begin alike.
inline constexpr std::array<unsigned char, 32> factory_head{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
struct Kind {
    const char* name;
    std::uint32_t manager; // in the renderer
    Fingerprint factory;
};
inline constexpr Kind kinds[]{
    {"OldPointLight", 0x00, {0x52d3f50, factory_head}},
    {"OldSpotLight", 0x000, {0x52d4480, factory_head}},
    {"PbrSphereLight", 0x000, {0x52d0e00, factory_head}},
    {"PbrSpotLight", 0x000, {0x52d1740, factory_head}},
    {"PbrTubeLight", 0x0000, {0x52d1d40, factory_head}},
    {"PbrRectangularLight", 0x0000, {0x52d2680, factory_head}},
};
}
