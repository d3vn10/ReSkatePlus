#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include "traffic_vehicles_rvas.h"

// The hierarchy pass for moving ECS entities. It can run on job-system threads.
namespace dingosdk::game::build::v20261013::traffic_vehicles {
inline constexpr std::array<unsigned char, 16> hierarchy_update_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// The mission override hooks the component query (mission_progression::component_query) that this
// code calls. Its first five bytes are four pushes, which Detours replaces with a five-byte jump
// (0x00), so once that hook is in only the prefix bytes after them still read as the game's
// (in game, take23: every take was refused on these bytes).
inline constexpr std::size_t component_query_hooked_bytes = 5;
inline constexpr unsigned char detours_jump = 0x00;
}
