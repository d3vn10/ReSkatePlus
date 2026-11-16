#pragma once
#include <cstdint>
#include "script_natives_rvas.h"
// Authored-graph natives ReSkate rebinds: the script error logger and SetFields.
namespace dingosdk::game::build::v20260929::script_natives {
// Script error logger native record (hash 0xa5910db3) and the shared no-op
// stub retail binds into its slots.
// SetFields native table row (hash 0xbdc6524d) and its function.
// The retail ownable-context resolve (native 0xdbfa3b23) is the stub
// FUN_1407bcbf0 that always reports failure; see the board-wear notes.
}
