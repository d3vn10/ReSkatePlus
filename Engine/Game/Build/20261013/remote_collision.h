#pragma once
#include "Engine/Game/Build/fingerprint.h"
#include <cstdint>
#include "remote_collision_rvas.h"

// Collision capsules for other players (Extension/Multiplayer/Remote/remote_collision.cpp).
// The engine's physics is EA FBPhysics: bodies and shapes are 16-byte handles
// {FBPhysicsWorld*, u32 index, u32 generation}; per-shape category masks live in the world
// state (world+0x00): InCategories at +0x000, CollidesWith at +0x000. Full notes:
// analysis/party-collision.md.
namespace dingosdk::game::build::v20261013::remote_collision {
// The client physics world: world(*(game_context_global)); what skater physics creation
// (0xf8d730) uses. Null while no level physics exists.

// FBPhysicsWorld::allocateBody(world, Handle* out).

// (body, motion type): 1 fixed, 2 keyframed, 3 dynamic.

// Two setters the retail skater proxy (0x1043390) calls on a new body: (body, 0.01f) writes
// the body's +0x000 value, (body) clears its +0x000 bit.


// (world, Handle* shape out, const Handle* body, const vec4* a, const vec4* b, float radius):
// a capsule from a to b in the body's frame.

// (shape, const u32* mask): the whole InCategories / CollidesWith mask.


// (shape, const Handle* owner): the shape's owner index (+0x0000), u32 at owner+8; -1 = none.
// The skater contact filter (0xf8cb80) resolves a contact's entity through it (0x25d6070).

// (body, const vec4* local centre of mass).

// (body, const float[8] {quaternion x y z w, position x y z w}).

// (body, const vec4* velocity), for any motion type (clamped to the body's maximum speed).
// 0x25da0b0 / 0x25d7fe0 are different: they only apply to dynamic bodies.



// Handle checks (index allocated and generation current).



// Category bits (PhysicsCollisionCategory; configurations/delmarphysicscategorypresets).
inline constexpr unsigned category_remote_character_proxy = 21, category_local_skater = 26,
                          category_character_proxy = 28;
// The other definitions whose names say character, ragdoll, skateboard or skater.
inline constexpr unsigned category_character = 2, category_ragdoll = 3, category_skateboard = 25,
                          category_remote_skater = 29;
// Retail's remote-skater proxy (0x10451f0 remote branch): In RemoteCharacterProxy,
// CollidesWith CharacterProxy. LocalSkater is added so the local rig (CollidesWith bit 21
// while party collision is on) meets it under either pair rule.
inline constexpr std::uint32_t remote_proxy_categories = 1u << category_remote_character_proxy;
inline constexpr std::uint32_t remote_proxy_collides_with =
    (1u << category_character_proxy) | (1u << category_local_skater);
// ProxySkater defaults (ctor 0x10371c0): height 1.8, radius 0.5, step 0.5; the capsule spans
// 0.15..1.45 m above the skater's root.
inline constexpr float proxy_radius = 0.5f, proxy_bottom = 0.15f, proxy_top = 1.45f;
// Local skater: the physics core (skater component +0x00, no_bail::bail_core_vtable) keeps
// the game's party-collision switch at +0x0000 (0x102aad0; "Enable Party Collision" AND
// DingoAdvanceSettings.PlayerVsPlayerCollisionEnabled).
} // namespace dingosdk::game::build::v20261013::remote_collision
