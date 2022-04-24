#include <stdio.h>
#include <Windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <conio.h>
#include <shlobj_core.h>
int main()
{
	DWORD processList[1024];
	CHAR dllPath[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, dllPath);
	strcat_s(dllPath, MAX_PATH, "\\pien.dll");

	printf("Waiting for game...\n");

	while (true)
	{
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		PROCESSENTRY32 info;
		info.dwSize = sizeof(PROCESSENTRY32);
		Process32First(snapshot, &info);
		do
		{
			HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, info.th32ProcessID);
			if (handle != NULL)
			{
				char name[MAX_PATH];
				GetModuleFileNameExA(handle, NULL, name, sizeof(name));
				if (strstr(name, "ElyonGameExecutor.exe"))
				{
					printf("Injecting...\n");
					LPVOID pDllPath = VirtualAllocEx(handle, NULL, strlen(dllPath) + 1, MEM_COMMIT, PAGE_READWRITE);
					WriteProcessMemory(handle, pDllPath, (LPVOID)dllPath, strlen(dllPath) + 1, 0);
					HANDLE thread = CreateRemoteThread(handle, NULL, NULL, (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "LoadLibraryA"), pDllPath, NULL, NULL);
					if (thread != NULL)
					{
						WaitForSingleObject(thread, INFINITE);
						VirtualFreeEx(handle, pDllPath, strlen(dllPath) + 1, MEM_RELEASE);
						printf("Xigncode bypassed. Press any key to exit.\n");
						_getch();
						return 0;
					}
				}
				CloseHandle(handle);
			}
		} while (Process32Next(snapshot, &info));
		CloseHandle(snapshot);
	}
}