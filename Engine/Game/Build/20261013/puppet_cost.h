#pragma once
#include "Engine/Game/Build/fingerprint.h"
#include <cstdint>
#include "puppet_cost_rvas.h"

// Native per-frame work that remote multiplayer puppets can skip or defer
// (Extension/Multiplayer/Remote/puppet_cost.cpp).
namespace dingosdk::game::build::v20260929::puppet_cost {
// Component update passes. Updatable components answer vtable +0x000..+0x000 ("wants
// PreSim, PostSim, PostFrame, PreInput, PreFrame, PreAnimation, PostAnimation") and are
// called at +0x000..+0x000 with (component, const UpdateInfo*). UpdateInfo is 0x00 bytes:
// +0 command buffer, +8 float dt (group time scale applied), +0x0 float raw dt.

// DelMarNavigatorComponent PreSim (vtable navigator_vtable + 0x000, shared with one other
// navigator class). Refreshes the actor's crowd-avoidance agent through 0x36d2190.

// StimBroadcastComponent PreSim (vtable stim_vtable + 0x000, shared with one other class):
// if dt > 0, broadcasts AI stimuli through 0x328e140(component + 0x00, dt).

// PedestrianColliderComponent PostSim (vtable collider_vtable + 0x000): velocity from the
// position change over dt, then moves its pedestrian-crowd collider.

// SkaterEcsInteropComponent PreSim (vtable ecs_vtable + 0x000). While component + 0x00 is
// clear (its pass test at vtable +0x000 is "+0x00 == 0") it queues command 0xf977b0, which
// retries the ECS prefab attach 0x105cfc0. It never completes on a remote actor, so the
// retry repeats every frame on both the skater and the skateboard.



// AntDrivenComponent (blueprint ClientIndex 4). Its init (vtable +0x00 = 0x2c37af0) registers
// item component + 0x00 with interval 1 in the per-context kind-6 manager stored at
// component + 0x00 (engine::source_manager_vtable; hooks object at manager + 0x00 ==
// manager + 0x00 with vtable ant_driven_hooks_vtable; kind at manager + 0x00). The manager
// update 0x2c3eee0 reads a joint of the rig's pose buffer (rig + 0x00, through 0x12e08d0) and
// re-places the entity with entity vtable +0x000 every frame. Its deinit (vtable +0x00 =
// 0x2c37870) unregisters with manager vtable +0x00 (manager_unregister).


inline constexpr std::uintptr_t update_info_size = 0x00;
inline constexpr std::uintptr_t update_info_dt = 8;
inline constexpr std::uintptr_t presim_handler_slot = 0x000;
inline constexpr std::uintptr_t postsim_handler_slot = 0x000;
inline constexpr std::uintptr_t ant_driven_item = 0x00;
inline constexpr std::uintptr_t ant_driven_manager = 0x00;
inline constexpr std::uintptr_t ant_driven_animatable = 0x00;
inline constexpr std::uintptr_t manager_hooks = 0x00;
inline constexpr std::uintptr_t manager_hooks_storage = 0x00;
inline constexpr std::uintptr_t manager_kind = 0x00;

}
