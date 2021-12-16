
#include "../../Include.hpp"

float IGlobalVarsBase::ServerTime(CUserCmd* cmd) noexcept
{
	static int tick;
	static CUserCmd* lastCmd;

	if (cmd) {
		if (G::Localplayer && (!lastCmd || lastCmd->bHasBeenPredicted))
			tick = G::Localplayer->GetTickBase();
		else
			tick++;
		lastCmd = cmd;
	}
	return tick * flIntervalPerTick;
}