#pragma once
#include "Engine/Game/Build/fingerprint.h"
#include <cstdint>
#include "native_menu_rvas.h"
namespace dingosdk::game::build::v20260929::native_menu {
// Native void-action descriptor copied for menu actions, and the invoker it
// holds at +0x00.

// The data model's record loader: Handle (manager, record, bool). It assumes DataModelObject at
// +18/+20 (Extension/UI/NativeMenu/native_menu_data.h; hook target).


// Type information
// Native widget blueprint asset.
// Exported asset record (styles and other list entries).
// Asset list holding exported records.

// UI textures (also used by the native compass indicator).
// Pointer to the RimeTextureAsset registry; its table and critical-section
// layout are verified by 0x4313bc0 / 0x4313cb0.

// Notes
// named_asset mirrors the native asset-domain parent traversal at 0x41b5780.
// Shutdown and multiplayer pause dumps fault at 0x3dc5eaa while destroying a
// menu item whose widget asset was unloaded.
}
