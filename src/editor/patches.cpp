#include "patches.h"

bool is_tab_disabled = false;
bool is_gravity_disabled = false;
bool is_enemy_movement_disabled = false;
bool is_enemy_loading_disabled = false;

void patches::Patch(uintptr_t dst_ptr, const BYTE asm_bytes[], size_t size) {
    DWORD oldProtect;
	LPVOID dst = (LPVOID)(base_addr + dst_ptr);

	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(dst, asm_bytes, size);
	VirtualProtect(dst, size, oldProtect, &oldProtect);
}

void patches::Nop(uintptr_t dst, size_t size)
{
	BYTE* nopArray = new BYTE[size];
	memset(nopArray, 0x90, size);
	Patch(dst, nopArray, size);
	delete[] nopArray;
}

void patches::DisableTab() {
    const BYTE tab_patch_asm[] = { 0x5E, 0x5D, 0x33, 0xC0, 0x5B, 0xC2, 0x10, 0x00 };
    Patch(p_tab, tab_patch_asm, sizeof(tab_patch_asm));
    is_tab_disabled = true;
}

void patches::EnableTab() {
    const BYTE tab_original_asm[] = { 0x8B, 0xF1, 0x0F, 0xB7, 0x46, 0x3A, 0xA8, 0x02 };
    Patch(p_tab, tab_original_asm, sizeof(tab_original_asm));
    is_tab_disabled = false;
}

void patches::DisableGravity() {
    Nop(p_gravity, 8);
    is_gravity_disabled = true;
}

void patches::EnableGravity() {
    const BYTE grav_original_asm[] = { 0xF3, 0x0F, 0x11, 0x87, 0x94, 0x18, 0x6E, 0x04 };
    Patch(p_gravity, grav_original_asm, sizeof(grav_original_asm));
    is_gravity_disabled = false;
}

void patches::DisableEnemyMovement() {
    Nop(p_enemy_movement, 5);
    is_enemy_movement_disabled = true;
}

void patches::EnableEnemyMovement() {
    const BYTE move_enemies_orig_asm[] = { 0xF3, 0x0F, 0x11, 0x40, 0x70 };
    Patch(p_enemy_movement, move_enemies_orig_asm, sizeof(move_enemies_orig_asm));
    is_enemy_movement_disabled = false;
}

void patches::DisableEnemyLoading() {
    Nop(p_enemy_loading, 5);
    is_enemy_loading_disabled = true;
}

void patches::EnableEnemyLoading() {
    const BYTE load_orig_asm[] = { 0xE8, 0xC9, 0x9E, 0x12, 0x00 };
    Patch(p_enemy_loading, load_orig_asm, sizeof(load_orig_asm));
    is_enemy_loading_disabled = false;
}

