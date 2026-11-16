#pragma once

// All September 29 executable addresses, grouped by the feature area that uses them.
// Include this when a build-level operation needs the complete RVA map; feature headers
// include their own companion *_rvas.h files directly.
#include "ai_skaters_rvas.h"
#include "camera_observer_rvas.h"
#include "engine_rvas.h"
#include "game_states_rvas.h"
#include "local_music_rvas.h"
#include "native_audio_rvas.h"
#include "native_cosmetics_rvas.h"
#include "native_indicators_rvas.h"
#include "native_menu_rvas.h"
#include "native_patch_support_rvas.h"
#include "native_player_ui_rvas.h"
#include "native_skater_rvas.h"
#include "native_throwdowns_rvas.h"
#include "profiler_labels_rvas.h"
#include "runtime_rvas.h"
#include "script_natives_rvas.h"
#include "skater_entities_rvas.h"
#include "skater_movement_rvas.h"
#include "skater_observer_rvas.h"
#include "skater_render_rvas.h"
#include "skater_state_rvas.h"
#include "ui_pointer_rvas.h"
#include "world_controls_rvas.h"
#include "world_model_rvas.h"
