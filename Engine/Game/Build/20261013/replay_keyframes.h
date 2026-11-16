#pragma once
#include "Engine/Game/Build/fingerprint.h"
#include <cstddef>
#include <cstdint>
#include "replay_keyframes_rvas.h"

// The replay editor's camera keyframes (Extension/ReplayKeyframes/replay_keyframes.h): framing keys
// (Behaviour, Subject, offsets, FOV), the free camera's among them, played as cubic Hermite segments
// between neighbouring keys. (The camera's other track, whole transforms played by 0x6222e0, isn't
// used by the free camera and is left alone.)
namespace dingosdk::game::build::v20260929::replay_keyframes {
// ---- Framing track: segments built when keys change ----
// rebuild(manager, session): any key edit ends here. It builds a segment for every framing key of the
// session with segment_build and sends the lot, with the transform keys, to the camera object. The
// manager sends one package at a time (its handle is at manager + 0x00): the session it rebuilt last
// is the one the camera plays.

// Segment* segment_build(manager, Segment* out, Node* keys_end, Node** key): the segment from a key
// to the next. keys_end is the anchor of the session's key map (session node + 0x00: [1] is the first
// key); the session's id is the int at session node + 0x00.


// void slot_unwrap(Slot* adjust, const Slot* reference, int behaviour): adjust's angles to within π
// of the reference's.

// bool segment_eval(camera, double time, const Segment*, Out*): plays a framing segment (every frame;
// not hooked: what it plays is decided when the segment is built), every lane through the game's
// spline (p0, p1, p2, p3, s, k1, k2):
//   m1 = (1 − k1)·0.5·(p2 − p0), m2 = (1 − k2)·0.5·(p3 − p1), Hermite from p1 to p2.

// A segment this short or shorter plays as its end (s = 1).
inline constexpr double tiny_gap = 1.0 / 65536.0;
// The manager singleton.
inline constexpr std::size_t manager_sessions = 0x00; // EASTL map<int, Session>: anchor; root at +0x00

// A key's node in a session's framing map: time at + 0x00, value at + 0x00.
inline constexpr std::size_t framing_key_time = 0x00;
inline constexpr std::size_t framing_key_value = 0x00;

// The segment (0x000 bytes): four slots of 0x00 bytes, two blocks of twelve k, Behaviour, Subject.
inline constexpr std::size_t slot_size = 0x00;
inline constexpr std::size_t slot_floats = slot_size / sizeof(float);
inline constexpr std::size_t slot_time = 0x00; // double
inline constexpr std::size_t segment_next = 0x000;     // p3, rescaled to one segment's time away
inline constexpr std::size_t segment_to = 0x000;       // p2
inline constexpr std::size_t segment_previous = 0x000; // p0, rescaled likewise
inline constexpr std::size_t segment_from = 0x000;     // p1
inline constexpr std::size_t segment_k_to = 0x000;     // twelve floats: the k of the to key
inline constexpr std::size_t segment_k_from = 0x000;   // and of the from key
inline constexpr std::size_t segment_k_count = 12;
inline constexpr std::size_t segment_behaviour = 0x000;
// The free camera's keys play under this Behaviour (with Subject 0): seen in game, 2026-10-16.
inline constexpr int free_camera_behaviour = 1;
inline constexpr std::size_t segment_subject = 0x000;
inline constexpr std::size_t segment_size = 0x000;

// Which slot floats each Behaviour plays, and with which k (segment_eval's evaluators 0x609a50,
// 0x608e10 and 0x609fe0, plus the FOV lane segment_eval plays itself).
struct Lane {
    unsigned char slot_float, k;
};
inline constexpr Lane behaviour_0_lanes[]{{0, 0}, {1, 1}, {2, 2}, {8, 4}, {9, 5}, {10, 6}, {18, 8}, {19, 9}, {21, 10}};
inline constexpr Lane behaviour_1_lanes[]{{12, 0}, {13, 1}, {14, 2}, {4, 4}, {5, 5}, {6, 6}, {21, 10}};
inline constexpr Lane behaviour_2_lanes[]{{0, 0}, {1, 1}, {2, 2}, {4, 4}, {5, 5}, {6, 6}, {18, 8}, {19, 9}, {21, 10}};
}
