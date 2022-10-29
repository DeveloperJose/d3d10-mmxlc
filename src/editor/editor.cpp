#include <unordered_map>

// Including Windows before detours is necessary
// https://stackoverflow.com/questions/31943646/getting-compilation-error-for-tlhelp32-h-in-mingw-g-compiler
// #include <Windows.h>
// #include "detours.h"
// #pragma comment(lib, "detours.lib")

#include "../../minhook/include/MinHook.h"

#include "pch.h"
#include "patches.h"
#include "x8_structs.h"
#include "editor.h"

namespace editor
{
	LPVOID ptr_enemy_load = (LPVOID)0x027e3d00;
	typedef int*(__thiscall *Enemy_Load)(PVOID, int32_t, int32_t, int32_t*, int32_t*, int32_t*, int32_t);
	// Enemy_Load TrueEnemyLoad = nullptr;
	Enemy_Load pReload = nullptr;
	Enemy_Load pReloadTarget = reinterpret_cast<Enemy_Load>(0x027e3d00);

	int32_t dw_esi = 0;
	// register int32_t dw_esi asm("esi");
	std::unordered_map<SetEnemyParent *, CEnemy *> enemy_map;
	int* __thiscall Hooked_Enemy_Load_Fn(void* ecx, int32_t param1, int32_t param2, int32_t* param3, int32_t* param4, int32_t* param5, int32_t param6)
	{
		// Save ESI to dw_esi
		// asm volatile("mov (dw_esi), %esi;");

		// Run original load function
		int* caddr = pReload(ecx, param1, param2, param3, param4, param5, param6);

		printf("EnemyLoad | ecx=%x param1=%x, param2=%x, param3=%x, param4=%x, param5=%x, param6=%x | ESI=%x, caddr=%x \n", &ecx, param1, param2, param3, param4, param5, param6, dw_esi, caddr);
		// Check if an enemy was loaded
		// CEnemy *c_enemy = (CEnemy *)caddr;
		// printf("ESI = %x and caddr = %x |\n", dw_esi, caddr);
		// printf("ESI = %x and caddr = %x | c_enemy=%s\n", dw_esi, caddr, c_enemy->type); // p1=%x, p4=%x, p6=%x
		// if (strcmp(c_enemy->type, "CEnemy") == 0 && dw_esi > (int32_t)base_addr)
		// {
		// 	// Map the SetEnemy parent to this CEnemy
		// 	SetEnemyParent *set_enemy_parent = (SetEnemyParent *)dw_esi;
		// 	enemy_map[set_enemy_parent] = c_enemy;

		// 	// Debugging
		// 	printf("CEnemy Loaded | %i enemies in map \n", enemy_map.size());
		// }
		return caddr;
	}

	Entity *entities = (Entity *)(base_addr + 0x42E1820);
	Entity *player1 = (Entity *)(base_addr + 0x42E21A0);
	Entity *player2 = (Entity *)(base_addr + 0x42E1950);

	int CountActiveEntities()
	{
		int active = 0;
		for (int i = 0; i <= 200; ++i)
		{
			if (entities[i].Animations1 != 0)
				++active;
		}
		return active;
	}

	SetFile *set_file = (SetFile *)(base_addr + 0x323F940);
	SetEnemyParent *set_parents = (SetEnemyParent *)(base_addr + 0x322B030);

	int32_t *pause_enemies = (int32_t *)(base_addr + 0x420A888);
	int32_t *pause_exploding_boxes = (int32_t *)(base_addr + 0x420A978);
	int32_t *pause_anims_and_interacts1 = (int32_t *)(base_addr + 0x420A8E8);
	int32_t *pause_anims_and_interacts2 = (int32_t *)(base_addr + 0x420A8D0);
	int32_t *pause_metals = (int32_t *)(base_addr + 0x420A8C4);
	int32_t *pause_fog = (int32_t *)(base_addr + 0x420A8AC);
	int32_t *hide_fog = (int32_t *)(base_addr + 0x420B6B4);
	int32_t *hide_hp_ui = (int32_t *)(base_addr + 0x420BE4C);

	int selected_set_enemy_idx = 0;

	CEnemy *selected_c_enemy = 0;
	int selected_entity_idx = 0;

	void MovePlayerToSetEnemy()
	{
		// Move player close to the coordinates the SetEnemy is supposed to be at
		SetEnemyParent *set_enemy_parent = &set_parents[selected_set_enemy_idx];
		SetEnemy *set_enemy = set_enemy_parent->set_enemy;
		for (int i = 0; i < 5; i++)
		{
			player1->X = set_enemy->x;
			player1->Y = set_enemy->y;
			player1->Z = set_enemy->z;
		}
		if (set_enemy_parent->is_active != 1)
		{
			printf("Trying to move player to an inactive enemy\n");
			return;
		}
		// Get the CEnemy that was last loaded
		selected_c_enemy = enemy_map[set_enemy_parent];
		selected_entity_idx = selected_c_enemy->entity_idx;
		// Sleep(250);

		// Hopefully by now the enemy has been loaded as an Entity. Let's look for it
		// selected_entity_idx = FindClosestEntity(set_enemy);
		// Entity* entity = &entities[selected_entity_idx];

		// Let's sync them a few times
		/*for (int i = 0; i < 10; i++) {
			player1->X = entities[selected_entity_idx].X;
			player1->Y = entities[selected_entity_idx].Y;
			player1->Z = entities[selected_entity_idx].Z;
			player1->Angle1 = entities[selected_entity_idx].Angle1;
			player1->Angle2 = entities[selected_entity_idx].Angle2;
			player1->Angle3 = entities[selected_entity_idx].Angle3;
			player1->Angle4 = entities[selected_entity_idx].Angle4;
			player1->Angle5 = entities[selected_entity_idx].Angle5;
			Sleep(5);
			//player1->Angle_BC = entities[selected_entity_idx].Angle_BC;
		}
		Sleep(5);*/
		printf("Current Entity = %x with ID = %i and coords (X=%f, Y=%f, Z=%f) | SetEnemy %s with IDX = %i and coords (X=%f, Y=%f, Z=%f, Angle=%f) | Player with coords (X=%f, Y=%f, Z=%f) | SetEnemyParent = %x, CEnemy = %x\n",
			   (uintptr_t)(&entities[selected_entity_idx]), entities[selected_entity_idx].ID, entities[selected_entity_idx].X, entities[selected_entity_idx].Y, entities[selected_entity_idx].Z,
			   set_enemy->prm_type, selected_set_enemy_idx, set_enemy->x, set_enemy->y, set_enemy->z, set_enemy->angle,
			   player1->X, player1->Y, player1->Z,
			   (uintptr_t)(&set_enemy_parent), (uintptr_t)(&selected_c_enemy));
	}

	bool is_editing = false;
	void ToggleEditMode()
	{
		is_editing = !is_editing;

		*pause_enemies = is_editing;
		*pause_exploding_boxes = is_editing;
		*pause_anims_and_interacts1 = is_editing;
		*pause_anims_and_interacts2 = is_editing;
		*pause_metals = is_editing;
		*pause_fog = is_editing;
		*hide_fog = is_editing;
		*hide_hp_ui = is_editing;

		if (is_editing)
		{
			patches::DisableGravity();
			patches::DisableEnemyMovement();
		}
		else
		{
			patches::EnableGravity();
			patches::EnableEnemyMovement();
		}
	}

	void Init(Overlay *overlay)
	{
		AllocConsole();
		freopen_s((FILE **)stdout, "CONOUT$", "w", stdout);
		freopen_s((FILE **)stderr, "CONOUT$", "w", stderr);
		freopen_s((FILE **)stdin, "CONIN$", "r", stdin);
		printf("[Editor.Init] Game Base Address=0x%x\n", base_addr);

		printf("[Editor.Init] Loading hooks\n");
		MH_STATUS status = MH_CreateHook(reinterpret_cast<void**>(pReloadTarget), (LPVOID)&Hooked_Enemy_Load_Fn, reinterpret_cast<void**>(&pReload));
		if (status == MH_OK){
			status = MH_EnableHook(reinterpret_cast<void**>(pReloadTarget));
			if (status != MH_OK){
				printf("[Editor.Init] EnableHook failed for Enemy_Load_Fn with status=%s\n", MH_StatusToString(status));
			}
		}
		else{
			printf("[Editor.Init] CreateHook failed for Enemy_Load_Fn with status=%s\n", MH_StatusToString(status));
		}

		// Detour enemy loading function
		// DetourTransactionBegin();
		// DetourUpdateThread(GetCurrentThread());
		// DetourAttach(&(PVOID&)TrueEnemyLoad, (PVOID)Hooked_Enemy_Load_Fn);
		// auto lError = DetourTransactionCommit();

		// if (lError != NO_ERROR) {
		// 	printf("Enemy load detour error %i\n", lError);
		// 	return;
		// }

		printf("[Editor.Init] Completed!\n");
	}

	void Shutdown() {
		FreeConsole();
	}
}