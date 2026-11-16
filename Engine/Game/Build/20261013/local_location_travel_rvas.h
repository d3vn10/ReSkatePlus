#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20261013::local_location_travel {
inline constexpr std::uintptr_t owner_vtable = 0x60d1e48;
inline constexpr std::uintptr_t location_type = 0x7251610;
inline constexpr std::uintptr_t access_point_type = 0x7251828;
inline constexpr std::uintptr_t access_point_record = 0x7f67560;
inline constexpr std::uintptr_t location_record = 0x7f676b0;
inline constexpr std::array<std::uintptr_t, 2> notify_event_types{0x72519e8, 0x7251a40};
inline constexpr std::array<std::uintptr_t, 2> byte_records{0x7f67330, 0x7f672c0};

}
