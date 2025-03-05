#include "pch.h"
#include <iostream>
#include "iat.h"

LPVOID hookiat(
	const char* _mod,
	const char* _func,
	LPVOID hookfunc,
	HMODULE targetmod) {
	if (!_mod || !_func || !hookfunc || !targetmod) return NULL;

	LPVOID img_base = targetmod;
	PIMAGE_DOS_HEADER dos_hdr = (PIMAGE_DOS_HEADER)img_base;
	PIMAGE_NT_HEADERS nt_hdr = (PIMAGE_NT_HEADERS)((DWORD_PTR)img_base + dos_hdr->e_lfanew);

	PIMAGE_IMPORT_DESCRIPTOR import_desc = NULL;
	IMAGE_DATA_DIRECTORY imp_dir = nt_hdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	import_desc = (PIMAGE_IMPORT_DESCRIPTOR)(imp_dir.VirtualAddress + (DWORD_PTR)img_base);

	LPCSTR lib_name = NULL;
	HMODULE lib = NULL;
	PIMAGE_IMPORT_BY_NAME func_name = NULL;
	LPVOID ret = NULL;

	while (import_desc->Name) {
		lib_name = (LPCSTR)(import_desc->Name + (DWORD_PTR)img_base);
		if (!_stricmp(_mod, lib_name)) {
			lib = GetModuleHandleA(_mod);
			FARPROC _target_func = NULL;
			if (lib)
				_target_func = GetProcAddress(lib, _func);

			if (lib && _target_func) {
				PIMAGE_THUNK_DATA original_first_thunk = NULL, first_thunk = NULL;
				original_first_thunk = (PIMAGE_THUNK_DATA)(import_desc->OriginalFirstThunk + (DWORD_PTR)img_base);
				first_thunk = (PIMAGE_THUNK_DATA)(import_desc->FirstThunk + (DWORD_PTR)img_base);

				while (original_first_thunk->u1.AddressOfData) {
					bool is_match = false;
					if (original_first_thunk->u1.AddressOfData & 0x80000000) {
						is_match = _target_func == (FARPROC)(first_thunk->u1.Function);
					}
					else {
						func_name = (PIMAGE_IMPORT_BY_NAME)(original_first_thunk->u1.AddressOfData + (DWORD_PTR)img_base);
						is_match = !_stricmp(func_name->Name, _func);
					}

					if (is_match) {
						DWORD old_protect;

						VirtualProtect((LPVOID)(&first_thunk->u1.Function), sizeof(LPVOID), PAGE_READWRITE, &old_protect);
						ret = (LPVOID)first_thunk->u1.Function;
						first_thunk->u1.Function = (DWORD_PTR)hookfunc;
						VirtualProtect((LPVOID)(&first_thunk->u1.Function), sizeof(LPVOID), old_protect, &old_protect);
					}

					original_first_thunk++;
					first_thunk++;
				}
			}
		}

		import_desc++;
	}

	return ret;
}