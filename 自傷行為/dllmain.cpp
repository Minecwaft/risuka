#include <Windows.h>
#include <MinHook.h>
#pragma comment(lib, "libMinHook.lib")

typedef int(WINAPI* CREATEPROCESSW)
(
	LPCWSTR,
	LPWSTR,
	LPSECURITY_ATTRIBUTES,
	LPSECURITY_ATTRIBUTES,
	BOOL,
	DWORD,
	LPVOID,
	LPCWSTR,
	LPSTARTUPINFOW,
	LPPROCESS_INFORMATION
);

CREATEPROCESSW OriginalCreateProcessW = nullptr;

BOOL WINAPI HookCreateProcessW(LPCWSTR lpApplicationName,
	LPWSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL inheritHandles,
	DWORD creationFlags,
	LPVOID lpEnviornment,
	LPCWSTR currentDirectory,
	LPSTARTUPINFOW startupInfo,
	LPPROCESS_INFORMATION processInformation)
{
	wchar_t commandline[500];
	wcscpy_s(commandline, lpCommandLine);
	wcscat_s(commandline, L" -noxigncode");
	return OriginalCreateProcessW
	(
		lpApplicationName,
		commandline,
		lpProcessAttributes,
		lpThreadAttributes,
		inheritHandles,
		creationFlags,
		lpEnviornment,
		currentDirectory,
		startupInfo,
		processInformation
	);
}

DWORD WINAPI Init(LPVOID param)
{
	if (MH_Initialize() != MH_OK)
	{
		FreeLibraryAndExitThread((HMODULE)param, 0);
		return 0;
	}
	if (MH_CreateHook(&CreateProcessW, &HookCreateProcessW, reinterpret_cast<LPVOID*>(&OriginalCreateProcessW)) != MH_OK)
	{
		FreeLibraryAndExitThread((HMODULE)param, 0);
		return 0;
	}
	if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
	{
		FreeLibraryAndExitThread((HMODULE)param, 0);
		return 0;
	}
	return 0;
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved) {
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(instance);
		CreateThread(NULL, NULL, Init, instance, NULL, NULL);
	}
	return TRUE;
}