#include "stdafx.h"
#include "stdio.h"
#include <windows.h>
#include "..\includes\injector\injector.hpp"
#include <cstdint>
#include <math.h>

float ZeroPointTwo = 0.2f;

DWORD PointerWeather, PointerWrite;

//DWORD WINAPI Thing(LPVOID);

bool(*CustomizeIsInBackRoom)() = (bool(*)())0x575B00;
unsigned int(__thiscall *CustomizeCategoryScreen_AddCustomOption)(void* TheThis, const char* unk1, unsigned int unk2, unsigned int unk3, unsigned int unk4) = (unsigned int(__thiscall*)(void*, const char*, unsigned int, unsigned int, unsigned int))0x007BB560;
unsigned int(__cdecl* FEngSetLanguageHash)(char const* FEngPackageName, unsigned int ObjectHash, unsigned int StringHash) = (unsigned int(__cdecl*)(char const*, unsigned int, unsigned int))0x525220;

DWORD PartsCustomGaugesCodeCaveExit = 0x007BBF8E;

void __declspec(naked) PartsCustomGaugesCodeCave()
{
	_asm
	{
		call CustomizeCategoryScreen_AddCustomOption
		call CustomizeIsInBackRoom
		test al,al
		jnz GaugesBackroom

	Gauges:
		mov edx, dword ptr ds: [0x00905EC4]
		push 0x307 // Gauges option
		push 0x78980A6B // Custom Gauges
		push 0x28F88BC // Gauges Icon
		push edx
		jmp CaveExit


	GaugesBackroom:
		mov edx, dword ptr ds : [0x00905EC4]
		push 0x307 // Gauges option
		push 0x78980A6B // Custom Gauges
		push 0x8BA602FC // Gauges Backroom Icon
		push edx


	CaveExit:
		mov ecx, esi
		call CustomizeCategoryScreen_AddCustomOption
		jmp PartsCustomGaugesCodeCaveExit
	}
}

DWORD ShowThreeButtons = 0x598FA0;
DWORD AutoDriveDialogCodeCaveExit = 0x55b277;

void __declspec(naked) AutoDriveDialogCodeCave()
{
	_asm
	{
		push 0x96AC0A32 // GPS?
		push 1
		push 0xB4EDEB6D
		push 0xB4EDEB6D
		push 0xA16CA7BD
		push 0x6CA522C5
		push 0x1A294DAD // Cancel
		push 0xAC49 // GPS
		push 0xCA0312 // AutoDrive
		push 3 // Selection
		push 0x89736C // InGameDialog.fng
		push eax
		call ShowThreeButtons
		jmp AutoDriveDialogCodeCaveExit
	}
}

DWORD j_malloc = 0x652AD0;
DWORD IconOption_Create = 0x586FA0;
DWORD IconScrollerMenu_AddOption = 0x573960;
DWORD ChallengeSeriesCodeCaveExit = 0x7AA821;

void __declspec(naked) ChallengeSeriesCodeCave()
{
	_asm
	{
		addpreviousoption:
			call IconScrollerMenu_AddOption

		addchallengeseries:
			push 0x4C
			call j_malloc
			mov edi, eax
			add esp, 04
			mov dword ptr ds : [esp + 0x8], edi
			test edi, edi
			mov dword ptr ds : [esp + 0x14], 00000001
			je csjump1
			push 0
			push 0xCC8CB746
			push 0x9A962438
			mov ecx, edi
			call IconOption_Create
			push ebx
			mov ebx, 0x901420
			mov dword ptr ds : [edi], ebx
			pop ebx
			jmp csjump2

		csjump1 :
			xor edi, edi

		csjump2 :
			mov ecx, esi
			push edi
			mov dword ptr ds : [esp + 0x18], 0xFFFFFFFF
			call IconScrollerMenu_AddOption

		caveexit :
			jmp ChallengeSeriesCodeCaveExit
	}
}

DWORD cFEng_QueuePackageSwitch = 0x525940;
DWORD ChallengeSeriesCodeCave2Exit = 0x7A2E8D;

void __declspec(naked) ChallengeSeriesCodeCave2()
{

	_asm
	{
		jz splitscreen
		dec eax
		jnz funcexit
		
		switchtochallengeseriesfng:
			pop edi
			pop esi
			mov dword ptr ds: [esp + 0x10], 0
			mov dword ptr ds : [esp + 0xC], 0
			mov dword ptr ds : [esp + 0x8], 0
			mov dword ptr ds : [esp + 0x4], 0x89CE1C // ChallengeSeries.fng
			mov ecx, dword ptr ds : [0x91CADC] // cFEng::mInstance
			jmp cFEng_QueuePackageSwitch

		funcexit :
			pop edi
			pop esi
			retn 0x10

		splitscreen:
			jmp ChallengeSeriesCodeCave2Exit
	}
}

void __declspec(naked) ChallengeSeries_React()
{

	_asm
	{
		cmp dword ptr ds : [esp + 0x8], 0x0C407210
		jnz funcexit
		mov dword ptr ds : [0x9B9E68], 3

		funcexit :
			retn 0x14
	}
}

void Init()
{
	// Move Gauges from Visual to Parts
	injector::MakeJMP(0x007BBF89, PartsCustomGaugesCodeCave, true);
	injector::MakeRangedNOP(0x7BC2F8, 0x7BC314, true); // Disable option in Visual
	injector::MakeRangedNOP(0x7BC355, 0x7BC372, true); // Backroom

	// Increase bounty every 5 seconds instead of 10
	injector::WriteMemory(0x444513, &ZeroPointTwo, true);
	injector::WriteMemory(0x444524, &ZeroPointTwo, true);

	// Light Streaks??
	injector::WriteMemory<BYTE>(0x9017E0, 1, true);

	// Autodrive dialog instead of GPS
	//injector::MakeJMP(0x55B24A, AutoDriveDialogCodeCave, true);

	// Challenge Series on Quick Race
	injector::MakeJMP(0x7AA81C, ChallengeSeriesCodeCave, true); // UIQRMainMenu::Setup
	injector::MakeJMP(0x544FE8, 0x545030, true); // Disable option in Main
	injector::MakeJMP(0x544E98, 0x544EDC, true); // Disable option in Main - 2 (MikeMannBuild)

	injector::WriteMemory<DWORD>(0x901420, 0x7B4950, true); // ChallengeSeries_vtbl
	injector::WriteMemory(0x901424, &ChallengeSeries_React, true);

	injector::MakeRangedNOP(0x7A2E87, ChallengeSeriesCodeCave2Exit, true);
	injector::MakeJMP(0x7A2E87, ChallengeSeriesCodeCave2, true); // UIQRMainMenu::NotificationMessage

	injector::MakeRangedNOP(0x7AE773, 0x7AE77B, true); // Quit To Quick Race Menu instead:
	injector::WriteMemory<DWORD>(0x7AE78F, 0x89D1F0, true); // UIQRChallengeSeries::NotificationMessage
	injector::WriteMemory<DWORD>(0x52B7A0, 0x4349998B, true); // PauseMenu::SetupOptions
	injector::WriteMemory<DWORD>(0x52C18B, 0x4349998B, true); // PauseMenu::SetupOptions

	// Fix Autosave (also save after quick race)
	injector::WriteMemory<BYTE>(0x5A7C72, 2, true); // PostRacePursuitScreen::NotificationMessage
	injector::WriteMemory<BYTE>(0x5A8260, 2, true); // PhotoFinishScreen::NotificationMessage
}

BOOL APIENTRY DllMain(HMODULE /*hModule*/, DWORD reason, LPVOID /*lpReserved*/)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		uintptr_t base = (uintptr_t)GetModuleHandleA(NULL);
		IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)(base);
		IMAGE_NT_HEADERS* nt = (IMAGE_NT_HEADERS*)(base + dos->e_lfanew);

		if ((base + nt->OptionalHeader.AddressOfEntryPoint + (0x400000 - base)) == 0x7C4040) // Check if .exe file is compatible - Thanks to thelink2012 and MWisBest
		{
			Init();
		}
		else
		{
			return FALSE;
		}
	}
	return TRUE;
}
