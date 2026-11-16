#pragma once
#include "Engine/Game/Build/fingerprint.h"
#include <cstdint>

#include "ui_model_rvas.h"

namespace dingosdk::game::build::v20261013::ui_model {
// Supported SHA-256 fbce74d5e28ef525dbba2cb4adbebc13405bdbd88f31bc940bca45e4ae88b8f9.
// The UI model's write of one value (manager, handle, type, value bytes, flag; true when written),
// found on 2026-10-07. The game's widget logic is data (expression functions: no native code names
// the fields), so its writes land here too. The publish the native menu reader uses (0x1924850,
// which takes the model lock at manager + 0x000 around it) and the model's other writers call it:
// the array element writer (0x1921e80), the path setter (0x1924570) and the data model setter
// (0x1923480), each under the same lock.

}
