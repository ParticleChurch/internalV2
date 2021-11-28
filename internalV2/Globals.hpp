#pragma once
#include "Entity.hpp"

namespace G {
	
	// Menu Crap
	extern bool MenuOpen;

	// The injected DLL's module
	extern HMODULE DLLModule;
	extern bool KillDLL;

	/*
	extern std::string CSGODirectory;
	extern std::string CSGOExePath;

	//General 
	extern QAngle StartAngle;
	extern QAngle EndAngle;
	extern bool IsInGame;
	extern bool FreezeTime;

	//LocalPlayer
	extern Entity* LocalPlayer;
	extern bool LocalPlayerAlive;
	extern int LocalPlayerIndex;
	extern int LocalPlayerTeam;
	extern Entity* LocalPlayerWeapon;
	extern WeaponData* LocalPlayerWeaponData;
	//maybe add flags and movetype?

	//CreateMove
	extern CUserCmd* cmd;
	extern bool* pSendPacket;
	extern float StartForwardMove;
	extern float StartSideMove;

	//Global Functions
	extern void CM_Clamp();
	extern void CM_Start(CUserCmd*, bool*);
	extern void CM_MoveFixStart();
	extern void CM_MoveFixEnd();
	extern void CM_End();

	// Pattern Scans / Convars
	extern ConVar* UpdateRate;
	extern ConVar* MaxUpdateRate;
	extern ConVar* Interp;
	extern ConVar* InterpRatio;
	extern ConVar* MinInterpRatio;
	extern ConVar* MaxInterpRatio;
	extern ConVar* MaxUnlag;
	extern DWORD AcceptMatchPattern;
	extern DWORD pD3d9DevicePattern;
	extern DWORD LoadSkyboxPattern;

	extern void GUIInit();
	extern void Init();
	*/
};