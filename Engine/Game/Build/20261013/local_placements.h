#pragma once
#include <array>
#include <cstdint>
#include "local_placements_rvas.h"

namespace dingosdk::game::build::v20260929::local_placements {
// Placed-object manager vtable; its id -> entity hash table starts at +0x00.
// uint32 registration offset of the server player's component holding the native
// simulation owner (+0x00). 0x7b3470/0x7befd0 activate rendering and collision
// only for that owner.
// uint32 offset of the client context record whose +0x000 byte is 1 once ready.
// Pointer to the teleport manager; its first uint32 is 0 while idle.
// The game's own ghosting of placed objects (walk-through, and solid again only once the
// skater is clear of them). The client keeps a 0x00-byte record per object; 0x7b3470 decides
// each one's ghost byte (+0x00) from a forced-solid byte (+0x00), a forced-ghost byte (+0x00),
// a mode (everyone solid, party and friends solid, only your own solid) and whether the local
// player is the native owner (0x7befd0). ReSkate's copies of other players' objects all have
// the local player as native owner, so only the forced bytes can ghost them.
// Expression native 0x00000000: void (const u64* entity, bool ghost). true forces the ghost;
// false lets go of a forced ghost (or, on an object that was not forced, forces it solid),
// and either way the record is decided again at once. Client realm, client thread.
// What that native does, in parts. It finds the record store through the calling thread's
// current context (0x18b6ae0 reads it from thread-local storage) plus client_ready_offset;
// the store is ready when its +0x000 byte is set and keeps its records at +0x00..+0x00.
// A record: entity u64 at +0x00, a group id at +0x00, active at +0x00, forced ghost +0x00,
// forced solid +0x00, a "decide now" byte +0x00, ghost +0x00, shown +0x00. 0x7b33e0 decides one
// record again: void (store*, record*). The rules it follows are the build kit settings
// (0x18d6ef0(*0x7482ce0, 0x723a510)): nothing is decided while their +0x00 byte is clear.
// A realm's context by its id: void* (context* out, u32 realm). ReSkate's placement updates run
// in the local server's context, so the client's store is reached through the client realm.
// Setting the store's +0x00 byte has the client decide every record again on its next update.
// A record's entity is the client's copy, with an id of its own. What the two sides share is the
// object's number: the index part of the server's entity (bits 8 up), which the client's
// component has at +0x00 and its records at +0x00 (the "group id" above).
inline constexpr std::array<unsigned char, 16> realm_context_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
inline constexpr std::uint32_t client_realm = 0x00000000, server_realm = 0x00000000;
inline constexpr std::array<unsigned char, 16> object_ghost_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// Expression native 0x00000000: bool (const u64* entity), whether the placed object that entity
// belongs to is the local player's (its component's owner at +0x00 against the local player's
// id). The object's menu is the owner's or a visitor's by it. object_root finds that object from
// any entity of it: void (u64 out[2], u64 entity, bool); out[0] is the object's own entity.
// object_component: void (const u64 object[2], u64 out[]); out[1] is set and out[2] is the
// placed-object component when the object has one.
// Expression native 0x00000000: bool (const u64 *const *entities), an array of placed objects'
// entities (count in the u32 before its first element, top bit apart). Whether all of them may
// be edited now; the owner is not part of it. The object card's script (flow_buildkit
// aa058fc, graph 0x00000000) puts the Edit button on the card by it, and Delete or
// Enable/Disable by object_owned.
inline constexpr std::array<unsigned char, 16> objects_editable_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// What was tried and left (2026-10-13): giving the client's copies their real owner (component
// +0x00). The game then hides them unless its store's +0x00 is set (its service's answer to
// whether others' objects may be seen) and their +0x00 is not 0x00, takes the far ones away
// (0x7c4c50), and looks the owner's card up by a model the session does not publish.
inline constexpr std::array<unsigned char, 16> object_component_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
inline constexpr std::array<unsigned char, 16> object_owned_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
inline constexpr std::array<unsigned char, 16> object_root_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// Reference only (PlacementCreateRecipe mirrors these, nothing calls them): the
// create-message handler 0x87da20; the free-roam drop 0x7af5b0, whose park
// context byte (127) comes from 0x6f7c750; the source enum codec 0x58f96c0/0x58f8c50.
}
