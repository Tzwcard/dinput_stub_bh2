#pragma once
#include <windows.h>

LPVOID hookiat(
	const char* _mod,
	const char* _func,
	LPVOID hookfunc,
	HMODULE targetmod);
