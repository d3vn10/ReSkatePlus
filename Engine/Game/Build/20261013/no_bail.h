#pragma once
#include <cstdint>
#include "no_bail_rvas.h"
#include "Engine/Game/Build/fingerprint.h"

namespace dingosdk::game::build::v20261013::no_bail {
// Supported SHA-256 fbce74d5e28ef525dbba2cb4adbebc13405bdbd88f31bc940bca45e4ae88b8f9.
// Skater-component arrival preference. Native 0xf9d170 copies on-board,
// stance and lip-mount bytes to +0x00..+0x00; the local teleport consumer at
// 0x566898 reads the on-board byte. We change only that byte, not stance/lip.
// void(collector, int32 reason, float magnitude): writes cause byte +0x00,
// XMM2 magnitude to +0x00, and increments cause count +0x00. Both the full
// wipeout and recoverable runout predicates consume this collector.


// Board placement entry (Dark Pop unpatch): `this` = board-placement object (+0x000 = physics context), rdx = arg
// (+0x00 flag, +0x00 side). Its entry test reads context+0x0000 bits 29/30 (mask 0x00000000).

// Board placement update (Dark Pop hold): called from the entry's special path every tick it is taken; `this` = the
// board-placement object (+0x000 hold active, +0x000 applied, +0x000 blend, +0x000 timer, +0x000 bone), rdx = arg.

// Board placement apply (Dark Pop hold): the per-tick placement the update runs while the hold is active; `this` =
// the board-placement object, rdx = the update's arg.

// Foot-attach request setters (Dark Pop hold): `this` = skater (board-placement +0x000), edx = mode. Each writes the
// mode into three fields of the skater's +0x0000 object (regular: +0x0000/84/88, bone 0x00; switch: +0x0000/74/78,
// bone 0x0) under the +0x0000 dirty flag. The pre-0.32.0 board placement called them with mode 2 during its hold.


// Footplant tick (Dark Pop, 1.3): state 601's per-tick controller, called from its step (0x4772ac0) on the placement
// object at state+0x00; rcx = that object (+0x000 context, +0x000 skater, +0x000 timer, +0x000 ticked), rdx = the
// step's output (+0 pos, +0x00 vel, +0x00 point, +0x00 jump, +0x00 flag, +0x00 valid). Before 0.32.0 it forked on the
// removed signal X: with X it moved the body on a Bezier and skipped the solver and the per-tick foot attaches.

// Skater mover (Dark Pop, 1.6): rcx = skater (rig), rdx = physics context, r8 = input (+0 position, +0x00 and +0x00
// two direction vectors, +0x00 byte), r9b = flag. Writes the input into the rig root (+0x0000..+0x0000), publishes the
// world transform (context +0, skater +0x0000), raises context+0x0000 bit 14 and poses the rig and, with the flag,
// its bodies. The footplant solver calls it every sink tick with the plant's position and orientation.



// The optimizer inlines several cause producers (for example +2a/+54/+e0
// at 0x474b27c), bypassing record_bail_cause_contract. Use the native per-step
// reset at consumption boundaries to cover those producers too. This complete
// leaf clears 35 cause bytes, 35 magnitudes and the count, leaving owner/config
// pointers and other collector fields untouched. It has no external calls.
inline constexpr std::array<unsigned char, 0x00> reset_bail_causes_code{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
// The post-physics skeleton response takes (rig, float dt, bool wipeout).
// The animation publisher takes the physics core. Both consume the explicit
// request at context+13c4 bit 15 and the independent request at +13d4 bit 27.


// Native consumers: skeleton caller's request test and ABI, animation's
// +87 explicit request export, and +8d independent request export. No Bail
// leaves the separate native recover/runout output at +9e alone.

// The sensitive body-contact report bypasses the cause collector. Native
// collision processing sets +0x0000 for configured body bones hitting a solid
// object; the rig publisher exports it to contacts+0x000. It is imported into
// context+0x0000 bit 22 (also tested by the Wipeout selector), and separately
// into the animation contact context at +0x00 by 0x102cbe0. Verify all three
// transfer sites. The independent recent-contact latch at +0x000 stays native.

inline constexpr LONG body_contact_context_mask = 0x400000;
inline constexpr LONG animation_request_mask = 0x00000000;
// These collision producers set context+13c4 bit 15 immediately before
// recording the cause. Animation reads that request independently of the cause
// collector; cancel both at these verified call sites, leaving other flags alone.
struct ImpactBail {
    Fingerprint caller;
    std::int32_t reason;
    std::uintptr_t return_rva;
};
inline constexpr std::array<ImpactBail, 3> impact_bail_calls{{
    {{0x47f2026, {
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}}, 13, 0x47f2045},
    {{0x47fa6cc, {
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}}, 9, 0x47fa6eb},
    {{0x47fa780, {
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}}, 14, 0x47fa79f},
}};

// Native recovery/stumble predicate. Its result is also exported to animation
// at +0x00, so No Bail never forces it. Reference only.
inline constexpr std::uintptr_t teleport_on_board_option = 0x00;
inline constexpr std::uintptr_t body_contact_output_offset = 0x000;
inline constexpr std::uintptr_t body_contact_context_offset = 0x0000;
inline constexpr std::uintptr_t animation_request_offset = 0x0000;
inline constexpr std::uintptr_t impact_request_offset = 0x0000;
inline constexpr std::uintptr_t recovery_animation_output = 0x00; // core+3b8 -> trick state
inline constexpr std::uintptr_t recovery_animation_flag = 0x00;   // animation output, byte

}
