#pragma once
#include <array>
#include <cstdint>
#include "unload_guard_rvas.h"

// Level unload crash guard. A job run while a level unloads walks each unloading object's table
// of 0x00-byte entries and reads a number at +0x00 of the asset each entry points to
// (`mov rax,[rbx+8]; and rax,-5; mov edx,[rax+0x00]`). An entry whose asset never resolved
// holds null there and the game crashes (seen leaving a mod map on a busy server). The loop is
// rewritten in place to skip such an entry: the 2-byte nop before it makes room for a `jz`
// straight after the `and`, the two moves the jump displaced follow it, and the loop's jump
// back is retargeted to the new start. Everything from the read on is where it was.
namespace dingosdk::game::build::v20260929::unload_guard {
inline constexpr std::array<unsigned char, 50> contract{
    0x00,0x00,                               // nop
    0x00,0x00,0x00,0x00,                     // loop: mov rax,[rbx+8]
    0x00,0x00,0x00,                          // mov r9,rbp
    0x00,0x00,0x00,0x00,                     // mov r8d,[rbx+0x00]
    0x00,0x00,0x00,0x00,                     // and rax,-5
    0x00,0x00,0x00,                          // mov rcx,rdi
    0x00,0x00,0x00,0x00,0x00,0x00,           // mov edx,[rax+0x00]
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,                     // next: add rbx,0x00
    0x00,0x00,0x00,0x00,0x00};               // cmp rbx,rsi; jne loop
inline constexpr std::array<unsigned char, 50> patch{
    0x00,0x00,0x00,0x00,                     // loop: mov rax,[rbx+8]
    0x00,0x00,0x00,0x00,                     // and rax,-5
    0x00,0x00,                               // jz next
    0x00,0x00,0x00,                          // mov r9,rbp
    0x00,0x00,0x00,0x00,                     // mov r8d,[rbx+0x00]
    0x00,0x00,0x00,                          // mov rcx,rdi
    0x00,0x00,0x00,0x00,0x00,0x00,           // mov edx,[rax+0x00]
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,                     // next: add rbx,0x00
    0x00,0x00,0x00,0x00,0x00};               // cmp rbx,rsi; jne loop (two bytes earlier)
// The jz sits at +8 and ends at +10; the entry's step is at +41.
static_assert(patch[9] == 41 - 10);
// The jne ends at +50 and the loop now starts at +0.
static_assert(static_cast<signed char>(patch[49]) == 0 - 50 && static_cast<signed char>(contract[49]) == 2 - 50);
}
