#pragma once
#include <cstdint>
#include "skater_body_rvas.h"
#include "Engine/Game/Build/fingerprint.h"

namespace dingosdk::game::build::v20260929::skater_body {
// Supported SHA-256 fbce74d5e28ef525dbba2cb4adbebc13405bdbd88f31bc940bca45e4ae88b8f9.
// The skater's body (Engine/Game/Skater/skater_body.h), per physics step: how hard each
// body hit something, and where its joint is. Measured in Ghidra on 2026-10-04; the ragdoll's skeleton publisher and its
// body maps were first found by gBGYo's ReSkate fork (github.com/gBGYo/ReSkate).
//
// The physics bone ids are the order of the game's physics bone name map (0xfd6600), one
// config field per name.
//
// The physics step of a skater's core (0x47da350) first processes the rig's body
// contacts (0x4777b60 -> process_body_contacts), then hands the same rig to the
// skeleton response (no_bail.h bail_skeleton_contract). So at the skeleton
// response the contact struct holds this step's values.

// rig+0x0000 -> holder, holder+0x0000 -> the contact struct process_body_contacts
// takes in RCX.


// process_body_contacts (0x4772d70) clears one 0x00-byte record per body bone at
// contact+0x000 at the start of every step: 0x00 bones, ids 0..23.

// For each contact of a bone id above 0 it keeps the step's largest impact speed
// in that bone's record: |dot(contact normal, other body velocity - bone velocity)|,
// metres per second along the normal (times a mass-ratio factor for a few contact
// kinds). +0x00 holds ordinary contacts; +0x00 the ones near one of two tracked points
// (their bones at contact+0x0000/0x0000: not only feet, the head was one in a bail).

// The bone record in full (0x477396f for ordinary contacts, 0x4773936 for those near a tracked
// point), written for a contact of a bone above 0 when it beats the record's peak: the contact
// normal (+0x00, tracked +0x00), the normal crossed with the relative velocity (+0x00, tracked
// +0x00: its length is how fast the bone moves along the surface, which the bails of 2026-10-06
// confirm), the contact point (+0x00) and the peak (+0x00, tracked +0x00). +0x00/+0x00 keep the
// contact's float +0x00, which stayed 0 in every contact of those bails.

// Then it marks the kind of what the bone hit (the other collidable's object +0x0000, else the
// contact's own +0x00): +0x00 kind 4, the board (the feet-on-board test asks for it); +0x00
// kind 8, vehicles (a bail into a car, 2026-10-06); +0x00 any other kind, the static world
// (ground, walls and a rail in those bails); +0x00 kind 5 and +0x00 kind 11, not met yet.

// Per bone besides the records: a byte at contact+0x000 is set while the bone touches something in
// this step, cleared at the start of each step and set by a contact (beside the bone's contact timer
// at contact+0x000, which a contact zeroes).


// Everything above, verified once before any of it is read.

// The core's step input (published at core+0x000 by core_set_pose_input): +0x00 is the length of the
// skater's physics step in seconds, copied to the context's +0x0000 every update (core_apply_step_input)
// and handed to the contacts and the skeleton. Taken from the simulation rate when the skater spawns
// or is teleported, never after (measured 2026-10-08).

inline constexpr std::uintptr_t core_step_input_offset = 0x000;   // core
inline constexpr std::uintptr_t step_input_length_offset = 0x00; // step input, float seconds
inline constexpr std::uintptr_t contact_holder_offset = 0x0000; // rig
inline constexpr std::uintptr_t contact_struct_offset = 0x0000; // holder
inline constexpr std::uintptr_t bone_records_offset = 0x000;     // contact struct
inline constexpr std::uintptr_t bone_record_size = 0x00;
inline constexpr std::uintptr_t bone_peak_offset = 0x00;      // float, ordinary contacts
inline constexpr std::uintptr_t bone_tracked_peak_offset = 0x00; // float, near a tracked point
inline constexpr std::uintptr_t body_transform_offset = 0x00;
inline constexpr std::uintptr_t bone_slide_offset = 0x00;        // float[4], and its tracked one at +0x00
inline constexpr std::uintptr_t bone_tracked_offset = 0x00;      // from an ordinary field to its tracked one
inline constexpr std::uintptr_t bone_hit_vehicle_offset = 0x00;  // bytes
inline constexpr std::uintptr_t bone_hit_kind_11_offset = 0x00;
inline constexpr std::uintptr_t bone_hit_kind_5_offset = 0x00;
inline constexpr std::uintptr_t bone_hit_world_offset = 0x00;
inline constexpr std::uintptr_t bone_hit_board_offset = 0x00;
inline constexpr std::uintptr_t bone_touching_offset = 0x000;    // contact struct, byte per bone

}
