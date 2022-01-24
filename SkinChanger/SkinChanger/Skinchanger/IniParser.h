/// IniParser.h
// Defines quick class to parse ini files with win api
#pragma once
#include "Common.h"
#pragma warning(disable:4800)

// Creds: TeamGamerFood, maybe? Can't remember exactly

class CIniParser
{
public:

	std::string szFile;

	std::string ReadString(const char* szSection, const char* szKey)
	{
		char szTemp[128];
		if (GetPrivateProfileStringA(szSection, szKey, "", szTemp, 128, szFile.c_str()))
		{
			char szString[128];
			sprintf(szString, szTemp);
			return std::string(szString);
		}
		return nullptr;
	}

	int ReadInt(const char* szSection, const char* szKey)
	{
		char szTemp[128];
		if (GetPrivateProfileStringA(szSection, szKey, "", szTemp, 128, szFile.c_str()))
		{
			char szString[128];
			sprintf(szString, szTemp);

			return std::stoi(szString);
		}
		return -1;
	}

	bool ReadBool(const char* szSection, const char* szKey)
	{
		char szTemp[128];
		if (GetPrivateProfileStringA(szSection, szKey, "", szTemp, 128, szFile.c_str()))
		{
			char szString[128];
			sprintf(szString, szTemp);
			return std::stoi(szString);
		}
		return false;
	}

	float ReadFloat(const char* szSection, const char* szKey)
	{
		char szTemp[128];
		if (GetPrivateProfileStringA(szSection, szKey, "", szTemp, 128, szFile.c_str()))
		{
			char szString[128];
			sprintf(szString, szTemp);

			return std::stof(szString);
		}
		return -1.f;
	}

	void Write(const char* szSection, const char* szKey, const char* szValue)
	{
		WritePrivateProfileStringA(szSection, szKey, szValue, szFile.c_str());
	}

	void WriteInt(const char* szSection, const char* szKey, int szValue)
	{
		WritePrivateProfileStringA(szSection, szKey, std::to_string(szValue).c_str(), szFile.c_str());
	}

	void WriteFloat(const char* szSection, const char* szKey, float szValue)
	{
		WritePrivateProfileStringA(szSection, szKey, std::to_string(szValue).c_str(), szFile.c_str());
	}

	bool SectionExists(const char* szSection)
	{
		char result[100];
		long lRetValue = GetPrivateProfileString(szSection, NULL, "", result, 90, szFile.c_str());
		return (lRetValue > 0);
	}

};

extern CIniParser* IniParser;