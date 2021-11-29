#include "../Include.hpp"

Movement* movement = new Movement();

void Movement::Bunnyhop()
{
	if (!cfg->Keybinds["Bunnyhop"].boolean) return;

	static int HopsMade = 0;
		
	// if we don't hold jump key and we are on the ground, then reset hops made
	if (!(G::cmd->iButtons & IN_JUMP) && G::LocalplayerAlive && (G::Localplayer->GetFlags() & FL_ONGROUND))
		HopsMade = -1;

	bool MakeHop = true;

	// deal with bhop chance
	if (cfg->movement.Chance != 100) {
		// 0 - 100
		MakeHop = (rand() % 100) < cfg->movement.Chance;
	}

	// make sure we make the minimum jumps
	if (cfg->movement.MinHops != 0 && HopsMade < cfg->movement.MinHops)
		MakeHop = true;
	 
	// make sure we aren't exceeding the maximum jumps
	if (cfg->movement.MaxHops != 0 && HopsMade >= cfg->movement.MaxHops)
		MakeHop = false;

	// if we aren't supposed to make the hop... force the person not to jump
	if (!MakeHop && G::Localplayer->GetFlags() & FL_ONGROUND && (G::cmd->iButtons & IN_JUMP)) {
		G::cmd->iButtons &= ~IN_JUMP;
		HopsMade = -1;
		return;
	}
		
	// if we are on a different tick and we are allowed to bhop...
	if ((G::cmd->iButtons & IN_JUMP) && G::LocalplayerAlive && !(G::Localplayer->GetFlags() & FL_ONGROUND)
		&& G::Localplayer->GetMoveType() != MOVETYPE_LADDER) {
		G::cmd->iButtons &= ~IN_JUMP;
	}

	// if we got here, that means that we are currently doing a bhop, so increase HopsMade
	if (G::cmd->iButtons & IN_JUMP)
		HopsMade++;

	
}
