/// PatternScan.h
// Defines a quick class to perform pattern scanning on client and engine modulez
#pragma once
#include "Common.h"

class CPatternScanner
{
private:

	HANDLE* m_hProcessToScan;
	MODULEENTRY32 m_ModuleToScan;
	BYTE* m_DumpedRegion; // WasserEsser don't hate me but I can't get this shit working completely with std::unique_ptr :^) I've tried tho
	DWORD m_dwModuleBase;
	DWORD m_dwModuleSize;

public:

	CPatternScanner(HANDLE* ProcessToScan, MODULEENTRY32 ModuleToScan)
		:	m_hProcessToScan(ProcessToScan),
			m_ModuleToScan(ModuleToScan),
			m_dwModuleBase((DWORD)ModuleToScan.modBaseAddr),
			m_dwModuleSize(ModuleToScan.modBaseSize)
	{
	}
	~CPatternScanner()
	{
		Clean();
	}

	bool Dump()
	{
		m_DumpedRegion = new BYTE[m_dwModuleSize];

		if (ReadProcessMemory(*m_hProcessToScan, (LPVOID)m_dwModuleBase, m_DumpedRegion, m_dwModuleSize, NULL) == FALSE)
			return false;

		return true;
	}
	bool MaskCheck(int iStartOffset, const BYTE* byPattern, const char* szMask)
	{
		for (unsigned int i=0; i<strlen(szMask); i++)
		{
			if (szMask[i] == '?') continue;
			if (szMask[i] == 'x' && byPattern[i] != this->m_DumpedRegion[iStartOffset + i])
				return false;
		}
		return true;
	}
	DWORD FindPattern(const BYTE* byPattern, const char* szMask, DWORD iAddValue = 0, bool bSubtractBase = false)
	{
		if (this->m_DumpedRegion==nullptr) if (!this->Dump()) return NULL;

		for (DWORD i=0; i < this->m_dwModuleBase + this->m_dwModuleSize; i++)
		{
			if (this->MaskCheck(i, byPattern, szMask))
				return bSubtractBase ? i + iAddValue : this->m_dwModuleBase + i + iAddValue;
		}

		return NULL;
	}

	void Clean()
	{
		delete [] m_DumpedRegion;
		nullptr;
	}

};

extern std::unique_ptr<CPatternScanner> ClientScanner;
extern std::unique_ptr<CPatternScanner> EngineScanner;