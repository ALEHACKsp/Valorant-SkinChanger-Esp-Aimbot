/// Memory.h
// Defines a quick class to better manage CS:GO memory externally
#pragma once
#include "Common.h"

class CMemoryManager
{
private:

	HANDLE m_hProcess;
	DWORD m_dwProcessID;
	MODULEENTRY32 m_Client;
	MODULEENTRY32 m_Engine;

public:

	bool AttachProcess(const std::string& strProcessName)
	{
		HANDLE hPID = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
		if (hPID == INVALID_HANDLE_VALUE) return false;

		PROCESSENTRY32 ProcEntry;
		ProcEntry.dwSize = sizeof(PROCESSENTRY32);

		// Mabye there should be a Process32First call here? hue

		do
			if (!strcmp(ProcEntry.szExeFile, strProcessName.c_str()))
			{
				this->m_dwProcessID = ProcEntry.th32ProcessID;
				CloseHandle(hPID);
				this->m_hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcEntry.th32ProcessID);
				return true;
			}
		while (Process32Next(hPID, &ProcEntry));

		CloseHandle(hPID);
		return false;
	}
	bool GetModule(const std::string& strModuleName)
	{
		// Kk, I filter to get only client.dll and engine.dll, this is shit ikr but I only need those 2 modules
		if (strModuleName != "client.dll" && strModuleName != "engine.dll")
			return false;

		HANDLE hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, this->m_dwProcessID);
		if (hModule == INVALID_HANDLE_VALUE) return false;

		MODULEENTRY32 ModEntry;
		ModEntry.dwSize = sizeof(MODULEENTRY32);

		// Maybe there should be a Module32First call here? hue

		do
			if (!strcmp(ModEntry.szModule, strModuleName.c_str()))
			{
				CloseHandle(hModule);
				(strModuleName == "client.dll") ? m_Client = ModEntry : m_Engine = ModEntry;
				return true;
			}
		while (Module32Next(hModule, &ModEntry));

		CloseHandle(hModule);
		return false;
	}

	template<class T> 
	T Read(DWORD dwAddress)
	{
		T TempValue;
		ReadProcessMemory(m_hProcess, (LPVOID)dwAddress, &TempValue, sizeof(T), NULL);
		return TempValue;
	}
	template<class T>
	T ReadSize(DWORD dwAddress, const size_t& dwSize)
	{
		T TempValue;
		ReadProcessMemory(m_hProcess, (LPVOID)dwAddress, &TempValue, dwSize, NULL);
		return TempValue;
	}
	template<class T>
	bool ReadArray(DWORD dwAddress, T* Array, const size_t& dwArraySize)
	{
		if (ReadProcessMemory(m_hProcess, (LPVOID)dwAddress, Array, sizeof(T) * dwArraySize, NULL) == FALSE)
		{
			Array = nullptr;
			return false;
		}
		return true;
	}
	std::string ReadString(DWORD dwAddress)
	{
		char* ch;
		ReadProcessMemory(m_hProcess, (LPVOID)dwAddress, ch, 255, NULL);
		std::string string = std::string(ch);

		std::string::size_type ind = string.find('\0');
		if (ind != std::string::npos)
			string = string.substr(0, ind);
		return string;
	}

	template<class T>
	bool Write(DWORD dwAddress, T ValueToWrite)
	{
		return WriteProcessMemory(m_hProcess, (LPVOID)dwAddress, &ValueToWrite, sizeof(T), NULL) == TRUE;
	}
	template<class T>
	bool WriteSize(DWORD dwAddress, T ValueToWrite, const size_t& dwSize)
	{
		return WriteProcessMemory(m_hProcess, (LPVOID)dwAddress, &ValueToWrite, dwSize, NULL) == TRUE;
	}
	template<class T>
	bool WriteArray(DWORD dwAddress, const T* ArrayToWrite, const size_t& dwArraySize)
	{
		return WriteProcessMemory(m_hProcess, (LPVOID)dwAddress, ArrayToWrite, sizeof(T) * dwArraySize, NULL) == TRUE;
	}

	HANDLE* GetHandle()
	{
		return (this->m_hProcess && this->m_hProcess != INVALID_HANDLE_VALUE) ? &this->m_hProcess : NULL;
	}
	DWORD GetProcessId()
	{
		return this->m_dwProcessID;
	}
	MODULEENTRY32 GetClient()
	{
		return this->m_Client;
	}
	MODULEENTRY32 GetEngine()
	{
		return this->m_Engine;
	}
	DWORD GetClientBase()
	{
		return (DWORD)this->m_Client.modBaseAddr;
	}
	DWORD GetEngineBase()
	{
		return (DWORD)this->m_Engine.modBaseAddr;
	}
	DWORD GetClientSize()
	{
		return this->m_Client.modBaseSize;
	}
	DWORD GetEngineSize()
	{
		return this->m_Engine.modBaseSize;
	}

	void Close()
	{
		CloseHandle(this->m_hProcess);
	}

};

extern std::unique_ptr<CMemoryManager> CSMemory;