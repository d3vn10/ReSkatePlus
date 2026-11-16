#pragma once
#include <cstdint>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20261013::entitlement_trace {
inline constexpr std::uintptr_t request_builder = 0x3912d0;
inline constexpr std::uintptr_t result_reducer = 0x76d900;
inline constexpr std::uintptr_t continuation_callback = 0x3f32b0;
inline constexpr std::uintptr_t entry_promise_capture = 0x774665;
inline constexpr std::uintptr_t entry_builder_call = 0x7746fe;
inline constexpr std::uintptr_t entry_builder_return = 0x77470f;
inline constexpr std::uintptr_t callback_reducer_call = 0x3f32b0;
inline constexpr std::uintptr_t callback_reducer_return = 0x76f858;

}
