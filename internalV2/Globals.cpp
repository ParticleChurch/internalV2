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

	int LocalplayerIndex;
	Entity* Localplayer = new Entity();
	bool LocalplayerAlive = false;
	int LocalplayerTeam;
}