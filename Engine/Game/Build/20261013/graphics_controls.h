#pragma once
#include <cstdint>
#include "graphics_controls_rvas.h"
#include "Engine/Game/Build/fingerprint.h"
#include "engine.h"

namespace dingosdk::game::build::v20260929::graphics_controls {
// Filmic component constructor, destructor and property setter; the settings
// manager lookup; the video filter pass setup.

// DingoVideoFilterRenderPass and its module; the pass caches the video effect mask.
// Filmic component vtable (destructor at +8, property setter at +0x00) and its data TypeInfo.
// GlobalPostProcessSettings TypeInfo. The native renderer settings service
// (3bc72f0) caches PostProcess copies by its DdcDirtyVersion.
// GlobalPostProcessSettings reflected field records.

}
