#include "pch.h"
#include <iostream>
#include <windows.h>
#include <dinput.h>
#include "hook.h"
#include "iat.h"

// MOUSE SCALE FOR X
#define MOUSE_MOVE_SCALE_X 4.0f

static int init_stub(void);
static int init_hook(void);

int do_hook(void) {
	int ret = 0;
	ret += init_stub() > 0 ? 1 : 0;
	ret += init_hook() > 0 ? 1 : 0;
	return ret;
}

/******************** HOOK ********************/
typedef BOOL(WINAPI* CallableGetCursorPos)(LPPOINT);
static CallableGetCursorPos GetCursorPosOrig = NULL;

BOOL WINAPI GetCursorPosStub(LPPOINT lpPoint) {
	BOOL ret = FALSE;
	LONG x = 0, y = 0;

	ret = GetCursorPosOrig(lpPoint);

	x = lpPoint->x;
	y = lpPoint->y;

	lpPoint->x = (LONG)((float)(x - 320) * MOUSE_MOVE_SCALE_X);

	SetCursorPos(320, y);

	return ret;
}

static int init_hook(void) {
	int ret = 0;

	// Hook IAT
	HMODULE hmod_bh2 = GetModuleHandleA("bh2.exe");

	if (hmod_bh2) {
		GetCursorPosOrig = (CallableGetCursorPos)hookiat(
			"user32.dll",
			"GetCursorPos",
			GetCursorPosStub,
			hmod_bh2
		);
		ret = 1;
	}

	return ret;
}

/******************** STUB ********************/
static HMODULE hmod = NULL;

typedef HRESULT(WINAPI* CallableDirectInputCreateA)(HINSTANCE, DWORD, LPDIRECTINPUT, LPUNKNOWN);
static CallableDirectInputCreateA orig = NULL;

HRESULT WINAPI DirectInputCreateA_stub(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUT lplpDirectInput, LPUNKNOWN punkOuter) {
	return orig(hinst, dwVersion, lplpDirectInput, punkOuter);
}

static int init_stub(void) {
	// Init stub function
	char path_sys[260];
	GetSystemDirectoryA(path_sys, 260);
	strcat_s(path_sys, "\\DINPUT.dll");
	hmod = LoadLibraryA(path_sys);

	if (hmod)
		orig = (CallableDirectInputCreateA)GetProcAddress(hmod, "DirectInputCreateA");

	return hmod && orig ? 1 : 0;
}