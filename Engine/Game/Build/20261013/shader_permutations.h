#pragma once
#include "Engine/Game/Build/fingerprint.h"
#include <array>
#include <cstdint>

// The game's expression shaders (Extension/World/shader_permutation_guard.cpp).
#include "shader_permutations_rvas.h"

namespace dingosdk::game::build::v20261013::shader_permutations {
// prepare(shader, renderer, state, bool, bool): run on a job worker (from 0x3f72b90, for every
// shader each view has registered, and from 0x3f79b70 for one). The shader keeps its compiled
// permutations as an array of pointers (begin at +0x00, end at +0x00). The function asks the
// first one for its kind (its first virtual function: ExpressionShaderRasterPermutation or
// ExpressionShaderRaytracePermutation) without checking it is there, then hands each to
// 0x46aa370 or 0x46aac20:
//   mov rax,[rcx+0x00]; ...; mov rcx,[rax]; mov rax,[rcx]   <- 0x3f86da6, rcx null

inline constexpr std::uintptr_t permutations_begin = 0x00, permutations_end = 0x00;
}
