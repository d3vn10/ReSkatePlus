#pragma once
#include "Engine/Game/Build/fingerprint.h"
#include <array>
#include <cstdint>

#include "body_contact_rvas.h"

namespace dingosdk::game::build::v20260929::body_contact {
// Static contact-vector readers and native point-motion arithmetic, whose layout the
// opt-in contact measurement (Engine/Game/World/body_contact_read.h) follows.
// Fingerprints alone do not qualify a world, body, result lifetime or actor exclusion.

inline constexpr std::uintptr_t result_positions = 0x00, result_normals = 0x00;
} // namespace dingosdk::game::build::v20260929::body_contact
