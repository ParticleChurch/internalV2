#include "../Include.hpp"

Aimbot* aimbot = new Aimbot();

#define DEBUG_AIMBOT true

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

void Aimbot::TimerStop(std::chrono::steady_clock::time_point start)
{
	auto end = std::chrono::steady_clock::now();

	auto diff = end - start;

	this->DeltaTime = std::chrono::duration <double, std::milli>(diff).count() / 1000.f;

	if (this->DeltaTime > (I::globalvars->flIntervalPerTick / 2.f))
	{
		MaxScan -= 5;
		MaxScan = max(2, MaxScan);
		ScanTimeBad = true;
	}
	else
	{
		ScanTimeBad = false;
		MaxScan = max(NumScan + 1, MaxScan);
	}

	MaxScan = min(MaxScan, cfg->performance.MaxAimbotScan);
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

float Aimbot::CalculateHitchance(Vector vangles, const Vector& point, Entity* player, int hbox)
{
	auto weapon = G::LocalplayerWeapon;
	if (!weapon)
		return 0.f;

	Vector forward, right, up;
	Vector eyepos = G::Localplayer->GetEyePosition();
	const auto spread = weapon->GetSpread();
	const auto inaccuracy = weapon->GetInaccuracy();
	const auto angles = vangles.Clamp();
	AngleVectors(angles, &forward, &right, &up);
	fast_vec_normalize(forward);
	fast_vec_normalize(right);
	fast_vec_normalize(up);
	Vector endpoint = point;

	float hits = 0;
	auto i = 0;

	// originally 255, reducing down to 100 for less accuracy, but quicker calc
	int NumTraces = cfg->performance.HitchanceScan;
	while (i < NumTraces)
	{
		const auto b = QuickRandom(0.f, 2.0f * 3.1415926535f); // tired of "warning C4305: 'argument': truncation from 'double' to 'float'"
		const auto c = QuickRandom(0.0f, 1.0f);
		const auto d = QuickRandom(0.f, 2.0f * 3.1415926535f); // so i swapped M_PI to this, lol

		const auto spread_val = c * spread;
		const auto inaccuracy_val = c * inaccuracy;

		const Vector v_spread((cos(b) * spread_val) + (cos(d) * inaccuracy_val), (sin(b) * spread_val) + (sin(d) * inaccuracy_val), 0);
		Vector dir;

		dir.x = forward.x + (right.x * v_spread.x) + (up.x * v_spread.y);
		dir.y = forward.y + (right.y * v_spread.x) + (up.y * v_spread.y);
		dir.z = forward.z + (right.z * v_spread.x) + (up.z * v_spread.y);

		fast_vec_normalize(dir);

		Vector spread_view;
		Vector end;
		vector_angles(dir, spread_view);
		spread_view.NormalizeNoClamp();
		Vector a = angles;
		a -= (spread_view - angles);
		AngleVectors(a, &end);
		fast_vec_normalize(end);

		Trace_t Trace;
		const auto trace_end = eyepos + (end * endpoint.Dist(eyepos));
		Ray_t Ray(eyepos, trace_end);
		I::enginetrace->ClipRayToEntity(Ray, MASK_SHOT_HULL | CONTENTS_HITBOX, (IHandleEntity*)player, &Trace);

		if (Trace.pHitEntity == player && HitboxToHitgroup(hbox) == HitboxToHitgroup(Trace.iHitbox)) // eg. in head
			hits += 1;

		i++;
	}

	return (hits / NumTraces);
}

float Aimbot::CalculatePsudoHitchance()
{
	if (!G::LocalplayerWeapondata)
		return 0;

	int a = WEAPONTYPE_C4;

	float inaccuracy = 0;
	int weapIndex = GetWeaponIndex(G::LocalplayerWeapon->GetWeaponId());

	switch (weapIndex)
	{
		// Pistols__________________________
	case 1: // glock
		inaccuracy = 0.0042f;
		break;
	case 2: // p2000
		inaccuracy = 0.003680f;
		break;
	case 3: // usp
		inaccuracy = 0.003680f;
		break;
	case 4: // deulies
		inaccuracy = 0.00525f;
		break;
	case 5: // P250
		inaccuracy = 0.006830f;
		break;
	case 6: // Tec9
		inaccuracy = 0.00368f;
		break;
	case 7: // 5-7
		inaccuracy = 0.00683f;
		break;
	case 8: // CZ
		inaccuracy = 0.0076f;
		break;
	case 9: // Deagle
		inaccuracy = 0.00218f;
		break;
	case 10: // Revolver
		inaccuracy = 0.005f;
		break;
		// Heavy__________________________
	case 11: // Nova
		inaccuracy = 0.00525f;
		break;
	case 12: // XM
		inaccuracy = 0.00525f;
		break;
	case 13: // SawedOFF
		inaccuracy = 0.00525f;
		break;
	case 14: // Mag7
		inaccuracy = 0.00525f;
		break;
	case 15: // M249
		inaccuracy = 0.00534f;
		break;
	case 16: // Negev
		inaccuracy = 0.00763f;
		break;
		// Smg__________________________
	case 17: // Mac10
		inaccuracy = 0.00998f;
		break;
	case 18: // MP9
		inaccuracy = 0.0055f;
		break;
	case 19: // MP7
		inaccuracy = 0.00592f;
		break;
	case 20: // MP5
		inaccuracy = 0.00592f;
		break;
	case 21: // UMP
		inaccuracy = 0.01007f;
		break;
	case 22: // P90
		inaccuracy = 0.01024f;
		break;
	case 23: // BIZON
		inaccuracy = 0.0105f;
		break;
		// Rifles__________________________
	case 24: // Galil
		inaccuracy = 0.00525f;
		break;
	case 25: // Famas
		inaccuracy = 0.00739f;
		break;
	case 26: // AK
		inaccuracy = 0.00481f;
		break;
	case 27: // M4A1
		inaccuracy = 0.0041f;
		break;
	case 28: // M4A1s
		inaccuracy = 0.0041f;
		break;
	case 29: // Scout
		inaccuracy = 0.0028f;
		break;
	case 30: // Sg553
		inaccuracy = 0.005f;
		break;
	case 31: // AUG
		inaccuracy = 0.00311f;
		break;
	case 32: // AWP
		inaccuracy = 0.0015f;
		break;
	case 33: // AUTO
		inaccuracy = 0.0015f;
		break;
	case 34: // AUTO
		inaccuracy = 0.0015f;
		break;
	default: // idk whatever
		inaccuracy = 0.f;
		break;
	}

	inaccuracy /= G::LocalplayerWeapon->GetInaccuracy();

	return inaccuracy;
}

void Aimbot::Run()
{
	if constexpr (DEBUG_AIMBOT) L::Debug("Run");

	if (!cfg->Keybinds["Aimbot"].boolean) return;

	if (!G::Localplayer->CanShoot()) return;

	if (CalculatePsudoHitchance() < 0.66f) return; //p100 hitchance

	// Start timer
	auto start = std::chrono::steady_clock::now();

	// reset the number of scans
	NumScan = 0;

	// go through lagcomp list
	for (auto& player_iterator : lagcomp->PlayerList)
	{
		// BREAK the moment we have scanned too much
		if (NumScan > MaxScan) break;

		// get the player
		auto& player = player_iterator.second;

		// ok checks
		if (player.records.empty()) continue;				// do record check
		if (player.LifeState != LIFE_ALIVE) continue;		// do live check
		if (!(player.Health > 0)) continue;					// do another live check
		if (player.Dormant) continue;						// do dormant check
		if (!player.records.front().bones.Valid) continue;  // bad front record bones

		// get correct record, also dumb
		auto& record = player.records[0];

		// look at the target bitch
		Vector Angle = Calc::CalculateAngle(record.bones.Head);
		Angle -= (G::Localplayer->GetAimPunchAngle() * 2);

		// traceray
		Trace_t ExitTrace;
		Ray_t Ray(G::Localplayer->GetEyePosition(), record.HeadPos);
		CTraceFilter filter((IHandleEntity*)G::Localplayer);
		I::enginetrace->TraceRay(Ray, MASK_SHOT_HULL, &filter, &ExitTrace);

		// we just did autowall scan
		NumScan++;

		// skip if trace isn't visible
		if (!ExitTrace.IsVisible()) continue;

		// silent aim
		G::cmd->angViewAngle = QAngle(Angle.x, Angle.y);

		// FIRE POW POW POW!
		G::cmd->iButtons |= IN_ATTACK;

		// deal with lag comp
		G::cmd->iTickCount = TimeToTicks(record.SimulationTime /*+ GetLerp()*/) - 1; // magic math 0_0

		// show the shot :D
		CapsuleOverlay(player.pEntity, Color(255, 0, 0, 255), 4, record.Matrix);

		// we found a good scan, shoot at it bitch
		break;
	}

	// do timer/scan analysis
	TimerStop(start);
}

void Aimbot::RenderStats()
{
	
}

/*
You can do a maximum of 500 hp damage to your teammates before you get kicked from the server.
However, overkill damage is also counted.
For example, if you kill a teammate with a head shot using an AWP, then it gets counted as 400+ damage rather than 100.
You can also get banned if you kill 3 teammates in the match.
*/