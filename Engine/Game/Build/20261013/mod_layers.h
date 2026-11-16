#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include "mod_layers_rvas.h"

namespace dingosdk::game::build::v20260929::mod_layers {
template<std::size_t N> consteval auto hex_bytes(const char (&hex)[N]) {
    static_assert(N % 2 == 1);
    std::array<std::uint8_t, (N - 1) / 2> result{};
    const auto nibble = [](char ch) -> unsigned {
        if (ch >= '0' && ch <= '9') return static_cast<unsigned>(ch - '0');
        if (ch >= 'a' && ch <= 'f') return static_cast<unsigned>(ch - 'a' + 10);
        throw "Invalid native contract hex";
    };
    for (std::size_t index = 0; index < result.size(); ++index)
        result[index] = static_cast<std::uint8_t>(
            (nibble(hex[index * 2]) << 4) | nibble(hex[index * 2 + 1]));
    return result;
}

// `classify` derives a layer kind from the layout.toc path.
inline constexpr auto classify_prefix = hex_bytes("4055535657488d6c24884881ec78010000488b052856fe024833c44889456048");
// `load` adds one layer to the layout manager.
inline constexpr auto load_layer_prefix = hex_bytes("48895c242055565741544157488dac2460feffff4881eca0020000488b053e05");
// Slot 0x00 of the layout manager's vtable, which must still be `load`.
// layout_bootstrap, which performs the LCU, Patch and Data loads in that order.
inline constexpr auto layout_bootstrap_prefix = hex_bytes("48895c2418488974242055574156488dac2430fdffff4881ecd0030000488b05");
}
