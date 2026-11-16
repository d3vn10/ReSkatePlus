#pragma once
#include <cstdint>
#include "world_props_rvas.h"

// Props spawned by ReSkate itself (Extension/Multiplayer/Remote/native_prop.cpp): an object
// blueprint loaded on the map, created as a client-only entity the way the remote skateboard is
// (skater_entities.h, native_skater.h) and moved every frame by the plain entity transform setter
// (skater_entities::place_entity). Found in game on 2026-10-15 with the traffic cone
// (world/props/common/construction/cmn_prp_trafficcone_01/cmn_prp_trafficcone_01): the creation
// list made one entity, and only place_entity moved it smoothly (the blueprint transform did not).
namespace dingosdk::game::build::v20260929::world_props {
// An ObjectBlueprint's vtable (the cone's). The skater code's blueprint vtable is
// engine::blueprint_vtable; an object blueprint has its own.
// The entity a static-model object blueprint creates on the client (the cone's).
} // namespace dingosdk::game::build::v20260929::world_props
