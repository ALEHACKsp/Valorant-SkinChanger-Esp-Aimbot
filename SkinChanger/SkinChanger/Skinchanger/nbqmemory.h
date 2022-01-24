#pragma once

/*

	CLASS THAT CONTAINS FUNCTIONS FOR MEMORY EDITING SUCH AS EASY-TO-USE WRAPPERS FOR ReadProcessMemory() and WriteProcessMemory()
	AND OTHER USEFUL FUNCTIONS FOR GETTING HANDLES AND PROCESS IDs VIA DIFFERENT METHODS THAT MIGHT BE SUITABLE FOR CERTAIN SITUATIONS.

	MADE BY NBQ ( 27th, 12, 2017 )

*/


#include <Windows.h>
#include <TlHelp32.h>

class NBQMemory
{
public:
	template <typename datatype>
	datatype ReadMemory(HANDLE hProcess, DWORD address)
	{
		datatype buffer;
		ReadProcessMemory(hProcess, (LPCVOID)address, &buffer, sizeof(datatype), NULL);
		return buffer;
	}

	template <typename datatype>
	void WriteMemory(HANDLE hProcess, DWORD address, datatype value)
	{
		WriteProcessMemory(hProcess, (LPVOID)address, &value, sizeof(value), NULL);
	}

	HANDLE GetHandleByProcessName(const char* processName, DWORD dwAccessRights)
	{
		DWORD pID = NULL;
		HANDLE hProcess = INVALID_HANDLE_VALUE;
		HANDLE ss = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
		if (ss != INVALID_HANDLE_VALUE)
		{
			PROCESSENTRY32 pe;
			pe.dwSize = sizeof(PROCESSENTRY32);
			do
			{
				if (!strcmp(pe.szExeFile, processName))
				{
					pID = pe.th32ProcessID;
					hProcess = OpenProcess(dwAccessRights, false, pID);
				}
			} while (Process32Next(ss, &pe));
			CloseHandle(ss);
		}
		return hProcess;
	}

	HANDLE GetHandleByWindowName(const char* windowName, DWORD dwAccessRights)
	{
		DWORD pID = NULL;
		HANDLE hProcess = INVALID_HANDLE_VALUE;
		HWND hW = FindWindowA(NULL, windowName);
		GetWindowThreadProcessId(hW, &pID);
		CloseHandle(hW);
		if (pID != NULL)
		{
			hProcess = OpenProcess(dwAccessRights, false, pID);
		}
		return hProcess;
	}

	DWORD GetProcessIdByProcessName(const char* processName)
	{
		DWORD pID = NULL;
		HANDLE ss = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
		if (ss != INVALID_HANDLE_VALUE)
		{
			PROCESSENTRY32 pe;
			pe.dwSize = sizeof(PROCESSENTRY32);
			do
			{
				if (!strcmp(pe.szExeFile, processName))
				{
					pID = pe.th32ProcessID;
				}
			} while (Process32Next(ss, &pe));
			CloseHandle(ss);
		}
		return pID;
	}

	DWORD GetProcessIdByWindowName(const char* windowName)
	{
		DWORD pID = NULL;
		HWND hW = FindWindowA(NULL, windowName);
		GetWindowThreadProcessId(hW, &pID);
		CloseHandle(hW);
		return pID;
	}

	DWORD GetModuleBaseAddress(DWORD pID, const char* moduleName)
	{
		DWORD ModuleBaseAddress = NULL;
		HANDLE ss = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID);
		if (ss != INVALID_HANDLE_VALUE)
		{
			MODULEENTRY32 me;
			me.dwSize = sizeof(MODULEENTRY32);

			if (Module32First(ss, &me))
			{
				do
				{
					if (!strcmp(me.szModule, moduleName))
					{
						ModuleBaseAddress = (DWORD)me.modBaseAddr;
						break;
					}
				} while (Module32Next(ss, &me));
			}
			CloseHandle(ss);
		}
		return ModuleBaseAddress;
	}

};