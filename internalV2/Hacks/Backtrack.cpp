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

static void CapsuleOverlay(Entity* pPlayer, Color col, float duration, matrix3x4_t pBoneToWorldOut[128])
{
	if (!pPlayer)
		return;

	studiohdr_t* pStudioModel = I::modelinfo->GetStudioModel(pPlayer->GetModel());
	if (!pStudioModel)
		return;

	mstudiohitboxset_t* pHitboxSet = pStudioModel->GetHitboxSet(0);
	if (!pHitboxSet)
		return;

	auto VectorTransform2 = [](const Vector in1, matrix3x4_t in2, Vector& out)
	{

		out.x = DotProduct(in1, Vector(in2[0][0], in2[0][1], in2[0][2])) + in2[0][3];
		out.y = DotProduct(in1, Vector(in2[1][0], in2[1][1], in2[1][2])) + in2[1][3];
		out.z = DotProduct(in1, Vector(in2[2][0], in2[2][1], in2[2][2])) + in2[2][3];
	};

	for (int i = 0; i < pHitboxSet->nHitboxes; i++)
	{
		mstudiobbox_t* pHitbox = pHitboxSet->GetHitbox(i);
		if (!pHitbox)
			continue;

		Vector vMin, vMax;
		VectorTransform2(pHitbox->vecBBMin, pBoneToWorldOut[pHitbox->iBone], vMin); //nullptr???
		VectorTransform2(pHitbox->vecBBMax, pBoneToWorldOut[pHitbox->iBone], vMax);

		if (pHitbox->flRadius > -1)
		{
			I::debugoverlay->AddCapsuleOverlay(vMin, vMax, pHitbox->flRadius, col.r(), col.g(), col.b(), 255, duration);
		}
	}
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
	CapsuleOverlay(I::entitylist->GetClientEntity(I::engine->GetPlayerForUserID(TargEnt)), Color(255, 165, 0, 255), 4, TargTick.Matrix);

}

Backtrack::Backtrack()
{
}
