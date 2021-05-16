#pragma once
#include "arcdps_structs_slim.h"

#include <string>
#include <Windows.h>

bool is_player(ag* new_player);

typedef uintptr_t (*WindowCallbackSignature)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
typedef uintptr_t (*CombatCallbackSignature)(cbtevent* ev, ag* src, ag* dst, const char* skillname, uint64_t id, uint64_t revision);
typedef uintptr_t (*ImguiCallbackSignature)(uint32_t not_charsel_or_loading);
typedef uintptr_t (*OptionsEndCallbackSignature)();
typedef uintptr_t (*OptionsWindowsCallbackSignature)(const char* windowname);

typedef struct arcdps_exports {
	uintptr_t size; /* size of exports table */
	uint32_t sig; /* pick a number between 0 and uint32_t max that isn't used by other modules */
	uint32_t imguivers; /* set this to IMGUI_VERSION_NUM. if you don't use imgui, 18000 (as of 2021-02-02) */
	const char* out_name; /* name string */
	const char* out_build; /* build string */
	WindowCallbackSignature wnd_nofilter; /* wndproc callback, fn(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) */
	CombatCallbackSignature combat; /* combat event callback, fn(cbtevent* ev, ag* src, ag* dst, char* skillname, uint64_t id, uint64_t revision) */
	ImguiCallbackSignature imgui; /* id3dd9::present callback, before imgui::render, fn(uint32_t not_charsel_or_loading) */
	OptionsEndCallbackSignature options_end; /* id3dd9::present callback, appending to the end of options window in arcdps, fn() */
	CombatCallbackSignature combat_local;  /* combat event callback like area but from chat log, fn(cbtevent* ev, ag* src, ag* dst, char* skillname, uint64_t id, uint64_t revision) */
	WindowCallbackSignature wnd_filter; /* wndproc callback like above, input filered using modifiers */
	OptionsWindowsCallbackSignature options_windows; /* called once per 'window' option checkbox, with null at the end, non-zero return disables drawing that option, fn(char* windowname) */
} arcdps_exports;
static_assert(sizeof(arcdps_exports) == 88, "");

typedef void* (*MallocSignature)(size_t);
typedef void (*FreeSignature)(void*);

typedef arcdps_exports* (*ModInitSignature)();
typedef uintptr_t (*ModReleaseSignature)();

struct IDirect3DDevice9;
struct ImGuiContext;
typedef ModInitSignature (*GetInitAddrSignature)(const char* arcversion, ImGuiContext* imguictx, IDirect3DDevice9* id3dd9, HMODULE arcdll, MallocSignature mallocfn, FreeSignature freefn);
typedef ModReleaseSignature (*GetReleaseAddrSignature)();

// additional enum for alignment
enum class Alignment {
	Left,
	Center,
	Right,
	Unaligned,

	// always last element
	FINAL_ENTRY
};

std::string to_string(Alignment alignment);
