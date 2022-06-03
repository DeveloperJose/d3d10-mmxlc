#ifndef EDITOR_PATCHES_H
#define EDITOR_PATCHES_H

#include "pch.h"

namespace patches {
    void Patch(uintptr_t dst_ptr, const BYTE asm_bytes[], size_t size);
    void Nop(uintptr_t dst, size_t size);

    void DisableTab();
    void EnableTab();
    const uintptr_t p_tab = 0x19DBC13;

    void DisableGravity();
    void EnableGravity();
    const uintptr_t p_gravity = 0x244E08D;

    void DisableEnemyMovement();
    void EnableEnemyMovement();
    const uintptr_t p_enemy_movement = 0x2439989;

    void DisableEnemyLoading();
    void EnableEnemyLoading();
    const uintptr_t p_enemy_loading = 0x23FF7B2;
}

#endif