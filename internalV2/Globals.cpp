#include "Include.hpp"

namespace G
{
	// menu crap
	bool MenuOpen = true;

	//for closing console
	HMODULE DLLModule = NULL;
	bool KillDLL = false;

	// Game stuff
	bool IsInGame = false;
	bool* pSendpacket;

	// Localplayer Stuff
	int LocalplayerIndex;
	Entity* Localplayer = new Entity();
	bool LocalplayerAlive = false;
	int LocalplayerTeam;
	Entity* LocalplayerWeapon;
	CCSWeaponData* LocalplayerWeapondata;

	// antiaim
	Vector real;
	Vector fake;

	// Createmove Stuff
	CUserCmd* cmd;
	QAngle StartAngle;
	float StartForwardmove;
	float StartSidemove;
	QAngle EndAngle;
}