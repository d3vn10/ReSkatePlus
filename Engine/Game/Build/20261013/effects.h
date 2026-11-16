#pragma once
#include <cstdint>
#include "effects_rvas.h"
#include "Engine/Game/Build/fingerprint.h"

namespace dingosdk::game::build::v20260929::effects {
// Supported Skate.exe SHA-256 fbce74d5e28ef525dbba2cb4adbebc13405bdbd88f31bc940bca45e4ae88b8f9.
// EffectEntity::onStart (29d0b40) and the skater VFX factory (486e720)
// construct the same 0x000-byte descriptor and own an 8-byte effect reference.
// Asset+8 is its TypeInfo; *TypeInfo is the record whose first word is the
// generated type identity, not native_name_hash(class name). Confirmed on the
// loaded private assets and in records 0x80c7870 / 0x8090360 of the pinned image.



// EffectEntity's 29cef80/29cd4e0 callers pass a 64-byte world matrix.



inline constexpr std::size_t descriptor_size = 0x000, descriptor_flags = 0x00;
inline constexpr std::uint32_t descriptor_flags_value = 0x00;
inline constexpr std::uintptr_t asset_compartment = 0x00;

inline constexpr std::array<std::uintptr_t, 4> service_methods{0x00, 0x00, 0x00, 0x00};

}
