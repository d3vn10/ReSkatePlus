#pragma once
#include <cstdint>
#include "script_natives_rvas.h"
// Authored-graph natives ReSkate rebinds: the script error logger and SetFields.
namespace dingosdk::game::build::v20261013::script_natives {
// Script error logger native record (hash 0x00000000) and the shared no-op
// stub retail binds into its slots.
// SetFields native table row (hash 0x00000000) and its function.
// The retail ownable-context resolve (native 0x00000000) is the stub
// FUN_1407bcbf0 that always reports failure; see the board-wear notes.
}
