#include "../Include.hpp"

#define DEBUG_BACKTRACK false

Backtrack* backtrack = new Backtrack();

void Backtrack::GetTargEnt()
{
	ValidEntFound = false;
	float BestTargDist = FLT_MAX;
	for (auto& a : lagcomp->PlayerList)
	{
		if (!(a.second.Health > 0)) continue;

		if (a.second.records.empty()) continue;

		float CrossDist = Calc::CrosshairDist(Calc::CalculateAngle(a.second.pEntity->GetVecOrigin()));
		if (CrossDist < BestTargDist)
		{
			BestTargDist = CrossDist;
			TargEnt = a.first;
			ValidEntFound = true;
		}
	}
}

void Backtrack::GetTargTick()
{
	ValidTickFound = false;
	float BestTargDist = FLT_MAX;
	for (auto& a : lagcomp->PlayerList[TargEnt].records)
	{
		float CrossDist = Calc::CrosshairDist(Calc::CalculateAngle(a.Origin));
		if (CrossDist < BestTargDist)
		{
			BestTargDist = CrossDist;
			TargTick = a;
			ValidTickFound = true;
		}
	}
}

void Backtrack::Run()
{
	if (!cfg->offense.Backtrack) return;

	if (!G::LocalplayerAlive) return;

	GetTargEnt();
	if (!ValidEntFound) return;

	if (!(G::cmd->iButtons & IN_ATTACK)) return;

	if (!G::LocalplayerAlive) return;

	if (!G::Localplayer->CanShoot()) return;

	GetTargTick();
	if (!ValidTickFound) return;

	float TargTime = TargTick.SimulationTime + GetLerp();
	float delta = fabsf(TargTime - I::globalvars->flCurrentTime);

	if (delta > cfg->offense.BacktrackTime / 1000.f)
		return;

	G::cmd->iTickCount = TimeToTicks(TargTime);
}

Backtrack::Backtrack()
{
}
