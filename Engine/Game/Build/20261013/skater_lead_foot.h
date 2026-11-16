#pragma once
#include <cstdint>
#include "skater_lead_foot_rvas.h"

namespace dingosdk::game::build::v20261013::skater_lead_foot {
// Supported game SHA-256 fbce74d5e28ef525dbba2cb4adbebc13405bdbd88f31bc940bca45e4ae88b8f9.
// Context +0x0000 bit 5 names the leading foot, not switch stance. Measured
// on goofy and regular riders (2026-10-20); retained when the old grind-exit
// module was removed from ReSkate 2.1.2. Base stance comes from the profile.
inline constexpr std::uint8_t left_foot_leads_mask = 0x00;
} // namespace dingosdk::game::build::v20261013::skater_lead_foot
