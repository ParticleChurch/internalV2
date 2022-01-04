#pragma once

namespace G {
	
	// Menu Crap
	extern bool MenuOpen;

	// The injected DLL's module
	extern HMODULE DLLModule;
	extern bool KillDLL;

	// Game stuff
	extern bool IsInGame;
	extern bool* pSendpacket;

	// Localplayer stuff
	extern int LocalplayerIndex;
	extern Entity* Localplayer;
	extern bool LocalplayerAlive;
	extern int LocalplayerTeam;
	extern Entity* LocalplayerWeapon;
	extern CCSWeaponData* LocalplayerWeapondata;

	// Antiaim stuff
	extern Vector real;
	extern Vector fake;

	// Createmove Stuff
	extern CUserCmd* cmd;
	extern QAngle StartAngle;
	extern float StartForwardmove;
	extern float StartSidemove;
	extern QAngle EndAngle;

	//LocalPlayer
	/*extern Entity* LocalPlayer;
	extern bool LocalPlayerAlive;
	extern int LocalPlayerIndex;
	extern int LocalPlayerTeam;*/
	//extern Entity* LocalPlayerWeapon;
	/*extern WeaponData* LocalPlayerWeaponData;*/
	//maybe add flags and movetype?


	// Createmove Data 
	/*extern QAngle StartAngle;
	extern QAngle EndAngle;
	extern bool IsInGame;
	extern bool FreezeTime;*/
	//CreateMove vars
	/*
	extern CUserCmd* cmd;
	extern bool* pSendPacket;
	extern float StartForwardMove;
	extern float StartSideMove;
	//CreateMove funcs
	extern void CM_Clamp();
	extern void CM_Start(CUserCmd*, bool*);
	extern void CM_MoveFixStart();
	extern void CM_MoveFixEnd();
	extern void CM_End();
	*/
};