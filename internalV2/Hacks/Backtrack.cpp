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

template<typename ...Args>
void ConsoleColorMsg(const Color& color, const char* fmt, Args ...args)
{
	using ConColorMsg = void(*)(const Color&, const char*, ...);
	static ConColorMsg con_color_msg = nullptr;
	if (!con_color_msg) {
		con_color_msg = reinterpret_cast<ConColorMsg>(GetProcAddress(
			GetModuleHandleA("tier0.dll"),
			"?ConColorMsg@@YAXABVColor@@PBDZZ")
			);
	}

	con_color_msg(color, fmt, args...);
}


void Backtrack::Run()
{
	if (!cfg->backtrack.Enable) return;

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

	if (delta > cfg->backtrack.BacktrackTime / 1000.f)
		return;

	if (delta > 0.1f)
		ConsoleColorMsg(Color(0, 255, 0, 255), ("Backtrack: " + std::to_string(delta) + "\n").c_str());

	G::cmd->iTickCount = TimeToTicks(TargTime);
}

Backtrack::Backtrack()
{
}
