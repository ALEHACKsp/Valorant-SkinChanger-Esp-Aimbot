/// main.cpp
// Entry point for the cheat
#include "Common.h"
#include "Memory.h"
#include "PatternScan.h"
#include "IniParser.h"
#define MAX_WEAPONS 64

using std::hex;
using std::dec;
using std::uppercase;
using std::nouppercase;
using std::cout;
using std::endl; // I'm lazy huh?

// Declare here the externed stuff (yeah I didn't do .h/.cpp because I'm a jerk hehe :DD)
std::unique_ptr<CMemoryManager> CSMemory = std::unique_ptr<CMemoryManager>(new CMemoryManager());
std::unique_ptr<CPatternScanner> ClientScanner;
std::unique_ptr<CPatternScanner> EngineScanner;
CIniParser* IniParser = new CIniParser();

// Patterns & Masks here
// Patterns by Y3t1Y3t and WasserEsser!!!
BYTE pt_LocalPlayer		[] = { 0xA3, 0x00, 0x00, 0x00, 0x00, 0xC7, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x59, 0xC3, 0x6A }; // + 0x1 ] - ClientBase + 0x10 )
const char* msk_LocalPlayer	= "x????xx????????x????xxx";
BYTE pt_EntityList		[] = { 0xBB, 0x00, 0x00, 0x00, 0x00, 0x83, 0xFF, 0x01, 0x0F, 0x8C, 0x00, 0x00, 0x00, 0x00, 0x3B, 0xF8 }; // + 0x1 ]
const char* msk_EntityList	= "x????xxxxx????xx";
BYTE pt_EnginePointer		[] = { 0xA1, 0x00, 0x00, 0x00, 0x00, 0xF3, 0x0F, 0x11, 0x80, 0x00, 0x00, 0x00, 0x00, 0xD9, 0x46, 0x04, 0xD9, 0x05 }; // + 0x1 ]
const char* msk_EnginePointer	= "x????xxxx????xxxxx";
BYTE pt_ForceFullUpdate		[] = { 0xB0, 0xFF, 0xB7, 0x00, 0x00, 0x00, 0x00, 0xE8 }; // + 0x3 ]
const char* msk_ForceFullUpdate	= "xxx????x";

// Offsets declaration here
DWORD m_dwLocalPlayer;
DWORD m_dwEntityList;
DWORD m_dwEnginePointer;
DWORD m_dwForceFullUpdate;

void FindOffsets()
{
	// Exactly, Wasser, I'm subtracting le bases uselessly :^))))
	m_dwLocalPlayer			= CSMemory->Read<DWORD>(ClientScanner->FindPattern(pt_LocalPlayer, msk_LocalPlayer) + 0x1) - CSMemory->GetClientBase() + 0x10;
	m_dwEntityList			= CSMemory->Read<DWORD>(ClientScanner->FindPattern(pt_EntityList, msk_EntityList) + 0x1) - CSMemory->GetClientBase();
	m_dwEnginePointer		= CSMemory->Read<DWORD>(EngineScanner->FindPattern(pt_EnginePointer, msk_EnginePointer) + 0x1) - CSMemory->GetEngineBase();
	m_dwForceFullUpdate		= CSMemory->Read<DWORD>(EngineScanner->FindPattern(pt_ForceFullUpdate, msk_ForceFullUpdate) + 0x3);
}
void PrintOffsets()
{
	cout << "OFFSETS:" << endl;
	cout << "m_dwLocalPlayer -> 0x" << hex << uppercase << m_dwLocalPlayer << dec << nouppercase << endl;
	cout << "m_dwEntityList -> 0x" << hex << uppercase << m_dwEntityList << dec << nouppercase << endl;
	cout << "m_dwEnginePointer -> 0x" << hex << uppercase << m_dwEnginePointer << dec << nouppercase << endl;
	cout << "m_dwForceFullUpdate -> 0x" << hex << uppercase << m_dwForceFullUpdate << dec << nouppercase << endl;
}

// Netvars here
DWORD m_hActiveWeapon = 0x2EE8;
DWORD m_hMyWeapons = 0x2DE8;
DWORD m_iItemDefinitionIndex = 0x2F88;
DWORD m_iAccountID = 0x2FA8;
DWORD m_OriginalOwnerXuidLow = 0x3160;
DWORD m_OriginalOwnerXuidHigh = 0x3164;
DWORD m_iItemIDLow = 0x2FA4;
DWORD m_iItemIDHigh = 0x2FA0;
DWORD m_nFallbackPaintKit = 0x3168;
DWORD m_nFallbackSeed = 0x316C;
DWORD m_flFallbackWear = 0x3170;
DWORD m_nFallbackStatTrak = 0x3174;
DWORD m_iEntityQuality = 0x2F8C;
DWORD m_szCustomName = 0x301C;

// Funcs here
void WelcomeScreen()
{
	ClearScreen();
	cout << "---- $$$ WELCOME TO EZSKINZ $$$ ---" << endl;
	cout << "|| [ F1 ] Set skins		 ||" << endl;
	cout << "|| [ F2 ] Print Config path	 ||" << endl;
	cout << "|| [ F3 ] Print instructions	 ||" << endl;
	cout << "|| [ F4 ] Close Cheat		 ||" << endl;
	cout << "-----------------------------------" << endl;
	cout << "|| Made by Double V for UnknownCheats.me" << endl;
	cout << "|| Credits to WasserEsser & Y3t1Y3t for signatures" << endl;
	cout << "-----------------------------------" << endl;
	cout << "Console Log:" << endl;
}

void ForceFullUpdate()
{
	DWORD EnginePointer = CSMemory->Read<DWORD>(CSMemory->GetEngineBase() + m_dwEnginePointer);
	CSMemory->Write<int>(EnginePointer + m_dwForceFullUpdate, -1);
}

struct Skin_t
{
	int PaintKit;
	int Seed;
	int StatTrak;
	int EntityQuality;
	char* CustomName;
	float Wear;

	Skin_t()
	{
		PaintKit = 0;
		Seed = 0;
		StatTrak = -1;
		EntityQuality = 4;
		CustomName = nullptr;
		Wear = 0.01f;
	}

	Skin_t(int PaintKit, int Seed, int StatTrak, int EntityQuality, char* CustomName, float Wear)
	{
		this->PaintKit = PaintKit;
		this->Seed = Seed;
		this->StatTrak = StatTrak;
		this->EntityQuality = EntityQuality;
		this->CustomName = CustomName;
		this->Wear = Wear;
	}
};

void WriteSkinConfig(int WeaponId, Skin_t Config)
{
	IniParser->WriteInt(std::to_string(WeaponId).c_str(), "PaintKit", Config.PaintKit);
	IniParser->WriteInt(std::to_string(WeaponId).c_str(), "Seed", Config.Seed);
	IniParser->WriteInt(std::to_string(WeaponId).c_str(), "StatTrak", Config.StatTrak);
	IniParser->WriteInt(std::to_string(WeaponId).c_str(), "EntityQuality", Config.EntityQuality);
	IniParser->Write(std::to_string(WeaponId).c_str(), "CustomName", Config.CustomName);
	IniParser->WriteFloat(std::to_string(WeaponId).c_str(), "Wear", Config.Wear);
}

// Main entry point
int main()
{
	SetConsoleTitle("$$ EZSKINZ BY DOUBLE V $$");
	HideCursor();
	COLOR(yellow);
	IniParser->szFile = ExePath() + "\\Config.ini";

	if (!FileExists(IniParser->szFile))
	{
		// write dummy ak config just to generate ini file
		WriteSkinConfig(7, Skin_t(180, 0, 1337, 1, "Fire Shit", 0.001f));
	}
	
	if (!CSMemory->AttachProcess("csgo.exe"))
	{
		FATAL_ERROR("CS:GO not found!");
		return 0;
	}

	// Spit out debug infos
	SEPARATOR();
	cout << "CS:GO found!" << endl;
	cout << "Handle value -> " << hex << uppercase << *CSMemory->GetHandle() << dec << nouppercase << endl;
	cout << "ProcessID -> " << hex << uppercase << CSMemory->GetProcessId() << dec << nouppercase << endl;

	if (!CSMemory->GetModule("client.dll"))
	{
		FATAL_ERROR("Client.dll not found!");
		return 0;
	}

	// Spit out more debug infos about client.dll
	SEPARATOR();
	cout << "Client.dll found!" << endl;
	cout << "Base Address -> " << hex << uppercase << CSMemory->GetClientBase() << dec << nouppercase << endl;
	cout << "Module Size -> " << CSMemory->GetClientSize() << " bytes"<< endl;

	if (!CSMemory->GetModule("engine.dll"))
	{
		FATAL_ERROR("Engine.dll not found!");
		return 0;
	}

	// Spit out more debug infos about engine.dll
	SEPARATOR();
	cout << "Engine.dll found!" << endl;
	cout << "Base Address -> " << hex << uppercase << CSMemory->GetEngineBase() << dec << nouppercase << endl;
	cout << "Module Size -> " << CSMemory->GetEngineSize() << " bytes"<< endl;

	ClientScanner = std::unique_ptr<CPatternScanner>(new CPatternScanner(CSMemory->GetHandle(), CSMemory->GetClient()));
	EngineScanner = std::unique_ptr<CPatternScanner>(new CPatternScanner(CSMemory->GetHandle(), CSMemory->GetEngine()));

	if (!ClientScanner->Dump())
	{
		FATAL_ERROR("Failed to dump client.dll!");
		return 0;
	}
	if (!EngineScanner->Dump())
	{
		FATAL_ERROR("Failed to dump engine.dll!");
		return 0;
	}

	SEPARATOR();
	FindOffsets();
	PrintOffsets();
	SEPARATOR();
	cout << "Debug ended, 3 secs and I launch the cheat :^)" << endl;
	SEPARATOR();

	std::this_thread::sleep_for(std::chrono::seconds(3));

	WelcomeScreen();

	while (!GetAsyncKeyState(VK_F4))
	{
		DWORD LocalPlayer = CSMemory->Read<DWORD>(CSMemory->GetClientBase() + m_dwLocalPlayer);

		// Thanks a lot WasserEsser for cleaning my mind about m_hMyWeapons... you are my hero :^)
		for (int i=0; i<MAX_WEAPONS; i++)
		{
			DWORD CurrentWeaponIndex = CSMemory->Read<DWORD>(LocalPlayer + m_hMyWeapons + ((i - 1) * 0x4)) & 0xFFF;
			DWORD CurrentWeaponEntity = CSMemory->Read<DWORD>(CSMemory->GetClientBase() + m_dwEntityList + (CurrentWeaponIndex - 1) * 0x10);
			int CurrentWeaponId = CSMemory->Read<int>(CurrentWeaponEntity + m_iItemDefinitionIndex);
			int MyXuid = CSMemory->Read<int>(CurrentWeaponEntity + m_OriginalOwnerXuidLow);
			CSMemory->Write<int>(CurrentWeaponEntity + m_iItemIDHigh, -1); //When iItemIDHigh is set to non zero value, fallback values will be used.
			if (IniParser->SectionExists(std::to_string(CurrentWeaponId).c_str()))
			{
				CSMemory->Write<int>(CurrentWeaponEntity + m_nFallbackPaintKit, IniParser->ReadInt(std::to_string(CurrentWeaponId).c_str(), "PaintKit"));
				CSMemory->Write<int>(CurrentWeaponEntity + m_nFallbackSeed, IniParser->ReadInt(std::to_string(CurrentWeaponId).c_str(), "Seed"));
				CSMemory->Write<int>(CurrentWeaponEntity + m_nFallbackStatTrak, IniParser->ReadInt(std::to_string(CurrentWeaponId).c_str(), "StatTrak"));
				CSMemory->Write<int>(CurrentWeaponEntity + m_iEntityQuality, IniParser->ReadInt(std::to_string(CurrentWeaponId).c_str(), "EntityQuality"));
				CSMemory->WriteArray<char>(CurrentWeaponEntity + m_szCustomName, IniParser->ReadString(std::to_string(CurrentWeaponId).c_str(), "CustomName").c_str(), IniParser->ReadString(std::to_string(CurrentWeaponId).c_str(), "CustomName").length());
				CSMemory->Write<float>(CurrentWeaponEntity + m_flFallbackWear, IniParser->ReadFloat(std::to_string(CurrentWeaponId).c_str(), "Wear"));
			}
			CSMemory->Write<int>(CurrentWeaponEntity + m_iAccountID, MyXuid);
		}

		if (GetAsyncKeyState(VK_F1))
			ForceFullUpdate();

		if (GetAsyncKeyState(VK_F2))
		{
			cout << "The Config Path is:" << endl << IniParser->szFile << endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(250));
		}

		if (GetAsyncKeyState(VK_F3))
		{
			cout << "Set the skins through the config.ini file" << endl;
			cout << "Then, press F1 while in game to set the skins" << endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(250));
		}

	//	std::this_thread::sleep_for(std::chrono::seconds(1)); You dont need this sleep
	}

	// Clean le stuffz out
	CSMemory->Close(); // Close handle to CS:GO
	return 0;
}
