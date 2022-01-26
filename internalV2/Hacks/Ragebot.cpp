#include "../Include.hpp"

Aimbot* rage = new Aimbot();

#define DEBUG_AIMBOT false

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

void Aimbot::TimerStart()
{
	this->StartTime = std::chrono::steady_clock::now();
}

bool Aimbot::TimerStop()
{
	auto end = std::chrono::steady_clock::now();

	auto diff = end - this->StartTime;

	this->DeltaTime = std::chrono::duration <double, std::milli>(diff).count() / 1000.f;

	if (this->DeltaTime > (I::globalvars->flIntervalPerTick / 2.f))
		return true;

	return false;
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

void Aimbot::SortPlayers(std::vector<std::pair<int, float>>& values)
{
	values.clear();
	values.resize(0);

	Vector localorigin = G::Localplayer->GetVecOrigin();
	for (auto a : lagcomp->PlayerList)
	{
		// If bad entity, continue
		if (a.second.Dormant) continue;
		if (!(a.second.Health > 0)) continue;
		if (!a.second.pEntity) continue;
		if (a.second.Team == G::LocalplayerTeam) continue;
		if (!(a.second.records.size() > 0)) continue;

		values.push_back(std::pair(a.first, (100 - a.second.Health) * 8 + 80000 / (a.second.records[0].Origin - localorigin).VecLength2D()));
	}

	//Trade secret sort struct for epic blazin fast aimbot
	struct {
		bool operator()(std::pair<int, float> a, std::pair<int, float> b) const
		{
			// otherwise sort by proper values
			return a.second > b.second;
		}
	} targetSort;

	// we do a bit of sortin
	std::sort(values.begin(), values.end(), targetSort);
}

void Aimbot::UpdateVals()
{
	this->MinHitchance = cfg->aimbot.Hitchance / 100.f;
	this->MinDamage = cfg->aimbot.MinDamage;
	this->HeadPointScale = cfg->aimbot.HeadPointScale / 100.f;
	this->BodyPointScale = cfg->aimbot.BodyPointScale / 100.f;
	
	// only update everytime we change our choice
	static int LastPriorityChoice = -1;
	if (cfg->aimbot.Priority != LastPriorityChoice)
	{
		LastPriorityChoice = cfg->aimbot.Priority;
		// clear the list
		this->Priority.clear();
		this->Priority.resize(0);

		// send to priority the relavent hitboxes
		switch (cfg->aimbot.Priority)
		{
		case 0:
			break;
		case 1:
			this->Priority.push_back(HITBOX_HEAD);
			break;
		case 2:
			this->Priority.push_back(HITBOX_NECK);
			break;
		case 3:
			this->Priority.push_back(HITBOX_PELVIS);
			break;
		case 4:
			this->Priority.push_back(HITBOX_STOMACH);
			break;
		case 5:
			this->Priority.push_back(HITBOX_THORAX); // aka lower chest
			break;
		case 6:
			this->Priority.push_back(HITBOX_CHEST);
			break;
		case 7:
			this->Priority.push_back(HITBOX_UPPER_CHEST);
			break;
		case 8:
			this->Priority.push_back(HITBOX_LEFT_THIGH);
			this->Priority.push_back(HITBOX_RIGHT_THIGH);
			break;
		case 9:
			this->Priority.push_back(HITBOX_LEFT_CALF);
			this->Priority.push_back(HITBOX_RIGHT_CALF);
			break;
		case 10:
			this->Priority.push_back(HITBOX_LEFT_FOOT);
			this->Priority.push_back(HITBOX_RIGHT_FOOT);
			break;
		case 11:
			this->Priority.push_back(HITBOX_LEFT_HAND);
			this->Priority.push_back(HITBOX_RIGHT_HAND);
			break;
		case 12:
			this->Priority.push_back(HITBOX_RIGHT_UPPER_ARM);
			this->Priority.push_back(HITBOX_LEFT_UPPER_ARM);
			break;
		case 13:
			this->Priority.push_back(HITBOX_RIGHT_FOREARM);
			this->Priority.push_back(HITBOX_LEFT_FOREARM);
			break;
		default:
			break;
		}
	}

	// update every second 
	static float oldTime = I::globalvars->flCurrentTime;
	if (fabsf(oldTime - I::globalvars->flCurrentTime) < 1)
		return;

	oldTime = I::globalvars->flCurrentTime;

	this->Hitboxes.clear();
	this->Hitboxes.resize(0);

	if (cfg->aimbot.Hitboxes[0])
		this->Hitboxes.push_back(HITBOX_HEAD);
	if (cfg->aimbot.Hitboxes[1])
		this->Hitboxes.push_back(HITBOX_NECK);
	if (cfg->aimbot.Hitboxes[2])
		this->Hitboxes.push_back(HITBOX_PELVIS);
	if (cfg->aimbot.Hitboxes[3])
		this->Hitboxes.push_back(HITBOX_STOMACH);
	if (cfg->aimbot.Hitboxes[4])
		this->Hitboxes.push_back(HITBOX_THORAX);
	if (cfg->aimbot.Hitboxes[5])
		this->Hitboxes.push_back(HITBOX_CHEST);
	if (cfg->aimbot.Hitboxes[6])
		this->Hitboxes.push_back(HITBOX_UPPER_CHEST);
	if (cfg->aimbot.Hitboxes[7])
	{
		this->Hitboxes.push_back(HITBOX_LEFT_THIGH);
		this->Hitboxes.push_back(HITBOX_RIGHT_THIGH);
	}
	if (cfg->aimbot.Hitboxes[8])
	{
		this->Hitboxes.push_back(HITBOX_LEFT_CALF);
		this->Hitboxes.push_back(HITBOX_RIGHT_CALF);
	}
	if (cfg->aimbot.Hitboxes[9])
	{
		this->Hitboxes.push_back(HITBOX_LEFT_FOOT);
		this->Hitboxes.push_back(HITBOX_RIGHT_FOOT);
	}
	if (cfg->aimbot.Hitboxes[10])
	{
		this->Hitboxes.push_back(HITBOX_LEFT_HAND);
		this->Hitboxes.push_back(HITBOX_RIGHT_HAND);
	}
	if (cfg->aimbot.Hitboxes[11])
	{
		this->Hitboxes.push_back(HITBOX_LEFT_UPPER_ARM);
		this->Hitboxes.push_back(HITBOX_RIGHT_UPPER_ARM);
	}
	if (cfg->aimbot.Hitboxes[12])
	{
		this->Hitboxes.push_back(HITBOX_LEFT_FOREARM);
		this->Hitboxes.push_back(HITBOX_RIGHT_FOREARM);
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

float Aimbot::GetPointScale(int Hitbox)
{
	switch (Hitbox)
	{
	case HITBOX_HEAD:
		return this->HeadPointScale;
		break;
	case HITBOX_CHEST:
	case HITBOX_THORAX:
	case HITBOX_UPPER_CHEST:
	case HITBOX_PELVIS:
	case HITBOX_STOMACH:
		return this->BodyPointScale;
	default:
		return 0.f;
	}
	// just in case a sneaky boi gets by (it wont but whatev)
	return 0.f;
}

float Aimbot::CalculatePsudoHitchance()
{
	if (!G::LocalplayerWeapondata)
		return 0;

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

/*
std::string get_active_window() {
	char window_title[256];
	GetWindowTextA(GetForegroundWindow(), window_title, 256);
	return window_title;
}
*/
Vector RotatePoint(Vector pos, Vector orig, float ang)
{
	// oreient ourselves wih origin @ the new origin
	pos = pos - orig;

	// cos/sin
	float COS = cos(DEG2RAD(ang));
	float SIN = sin(DEG2RAD(ang));

	// rotate point around origin
	Vector FinalPos;
	FinalPos.x = pos.x * COS - pos.y * SIN;
	FinalPos.y = pos.y * COS + pos.x * SIN;
	FinalPos.z = pos.z;

	// orient ourselves back from given origin
	FinalPos += orig;
	return FinalPos;
}

bool GetSafepoint(Vector origin, Vector point, float radius, Vector& safepoint, float MaxDesync)
{
	origin.z = point.z;
	Vector r1 = RotatePoint(point, origin, 180);
	Vector r2 = RotatePoint(point, origin, 180 + MaxDesync);
	Vector r3 = RotatePoint(point, origin, 180 - MaxDesync);

	std::vector<Vector> pts = { r1, r2, r3 };

	return FoundSafepoint(G::Localplayer->GetEyePosition(), pts, radius, safepoint);
}

bool SAFEPOINT(LagComp::Record& record, float radius, Vector min, Vector max, Vector& aimpoint)
{
	// Go for max first
	if (GetSafepoint(record.Origin, max, radius, aimpoint, record.MaxDesync))
		return true;
	// then try for middle, min just fucks shit up
	if (GetSafepoint(record.Origin, (max + min) / 2, radius, aimpoint, record.MaxDesync))
		return true;
	return false;
}

void Aimbot::Run()
{
	if constexpr (DEBUG_AIMBOT) L::Debug("Run Aimbot");

	if (!cfg->Keybinds["Aimbot"].boolean) return;

	if (!G::Localplayer->CanShoot()) return;

	if (antiaim->LaggingOnPeak) return;

	UpdateVals();

	// Start timer
	TimerStart();

	// reset the number of scans
	NumScan = 0;

	SortPlayers(this->players);

	// go through lagcomp list
	for (auto& player_userid : this->players)
	{
		// STOP AIMBOT the moment we have scanned too much
		if (TimerStop()) goto END_AIM;

		// get the player
		auto& player = lagcomp->PlayerList[player_userid.first];

		// ok checks
		if (player.records.empty()) continue;				// do record check
		if (player.LifeState != LIFE_ALIVE) continue;		// do live check
		if (!(player.Health > 0)) continue;					// do another live check
		if (player.Dormant) continue;						// do dormant check

		// get given record
		auto& record = player.records[0];

		// check if valid bones
		if (!record.ValidBones) continue;

		// priority scan
		if constexpr (DEBUG_AIMBOT) L::Debug("priority hitbox scan");
		for (auto HITBOX : this->Priority)
		{
			// BREAK the moment we have scanned too much
			if (TimerStop()) goto END_AIM;

			//Vector Aimpoint = record.HeadPos;// Calc::ExtrapolateTick(record.HeadPos, record.Velocity, 4);
			Vector min = record.bones[HITBOX].vecBBMin.Transform(record.Matrix[record.bones[HITBOX].iBone]);
			Vector max = record.bones[HITBOX].vecBBMax.Transform(record.Matrix[record.bones[HITBOX].iBone]);
			Vector mid = (min + max / 2);
			Vector top = min.z > max.z ? min : max;
			float PointScale = GetPointScale(HITBOX);
			Vector top_left = player.pEntity->GetTopLeft(top, record.bones[HITBOX].flRadius * PointScale, G::Localplayer);
			Vector top_right = player.pEntity->GetTopRight(top, record.bones[HITBOX].flRadius * PointScale, G::Localplayer);
			Vector Aimpoint = G::cmd->iTickCount % 2 ? top_left : top_right;

			// safepoint when possible for head
			bool safepoint = false;
			if (HITBOX == HITBOX_HEAD)
			{
				safepoint = SAFEPOINT(record, record.bones[HITBOX].flRadius, min, max, Aimpoint);
			}
				

			// angle to the target bitch
			Vector Angle = Calc::CalculateAngle(Aimpoint);
			Angle -= (G::Localplayer->GetAimPunchAngle() * 2);

			// shit hitchance, nah
			if constexpr (DEBUG_AIMBOT) L::Debug("hitchance");
			float hitchance = 0;
			/*if (i == 0)
				hitchance = CalculateHitchance(Angle, Aimpoint, player.pEntity, HITBOX);
			else*/
			hitchance = CalculatePsudoHitchance();
			if (hitchance < this->MinHitchance) continue;

			if constexpr (DEBUG_AIMBOT) L::Debug("GetDamage");
			float Damage = 0;
			Damage = autowall->GetDamage(G::Localplayer, Aimpoint);

			L::Debug(("Autowal Damage: " + std::to_string(Damage)).c_str());

			// we just did autowall scan
			NumScan++;

			// damage check
			if (Damage < this->MinDamage) continue;

			// log that shot damage :D
			/*H::console.push_back(GetHitboxStr(HITBOX) + " " + std::to_string((int)Damage));
			while (H::console.size() > 10)
				H::console.erase(H::console.begin());*/

				// silent aim
			G::cmd->angViewAngle = QAngle(Angle.x, Angle.y);

			// FIRE POW POW POW!
			G::cmd->iButtons |= IN_ATTACK;

			// deal with lag comp
			if constexpr (DEBUG_AIMBOT) L::Debug("iTickCount");
			G::cmd->iTickCount = TimeToTicks(record.SimulationTime /*+ GetLerp()*/) - 1; // magic math 0_0

			if (!cfg->Keybinds["Fake Duck"].boolean)
				*G::pSendpacket = true;

			if constexpr (DEBUG_AIMBOT) L::Debug("CapsuleOverlay");
			// show the shot :D
			//CapsuleOverlay(player.pEntity, Color(255, 0, 0, 255), 4, record.Matrix);

			/*std::string msg = "Shot at [" + (std::string)player.Info.szName + "] with hitchance [" + std::to_string(hitchance) + "]\n";
			ConsoleColorMsg(Color(0, 255, 0, 255), msg.c_str());*/

			// we found a good scan, shoot at it bitch
			Resolver::aim_shot_log shot;
			shot.userID = player_userid.first;
			shot.time = record.SimulationTime;//I::globalvars->flRealTime;
			shot.hitgroup = HitboxToHitgroup(HITBOX);
			shot.damage = Damage;
			shot.shootPos = G::Localplayer->GetEyePosition();
			shot.shootPoint = Aimpoint;
			shot.origin = record.Origin;
			shot.min_bone = min;
			shot.max_bone = max;
			shot.radius_bone = record.bones[HITBOX].flRadius;
			shot.safepoint = safepoint;
			resolver->AimbotShot(shot);

			goto END_AIM;
		}

		// general hitbox scan
		if constexpr (DEBUG_AIMBOT) L::Debug("general hitbox scan");
		for (auto HITBOX : this->Hitboxes)
		{
			// Dont scan something that we have already scanned in priority
			bool skip = false;
			for (auto item : this->Priority)
			{
				if (item == HITBOX)
					skip = true;
			}
			if (skip)
				continue;

			// BREAK the moment we have scanned too much
			if (TimerStop()) goto END_AIM;

			//Vector Aimpoint = record.HeadPos;// Calc::ExtrapolateTick(record.HeadPos, record.Velocity, 4);
			Vector min = record.bones[HITBOX].vecBBMin.Transform(record.Matrix[record.bones[HITBOX].iBone]);
			Vector max = record.bones[HITBOX].vecBBMax.Transform(record.Matrix[record.bones[HITBOX].iBone]);
			Vector mid = (min + max / 2);
			Vector top = min.z > max.z ? min : max;
			float PointScale = GetPointScale(HITBOX);
			Vector top_left = player.pEntity->GetTopLeft(top, record.bones[HITBOX].flRadius * PointScale, G::Localplayer);
			Vector top_right = player.pEntity->GetTopRight(top, record.bones[HITBOX].flRadius * PointScale, G::Localplayer);
			Vector Aimpoint = G::cmd->iTickCount % 2 ? top_left : top_right;

			bool safepoint = false;
			if (HITBOX == HITBOX_HEAD)
			{
				safepoint = SAFEPOINT(record, record.bones[HITBOX].flRadius, min, max, Aimpoint);
			}

			// angle to the target bitch
			Vector Angle = Calc::CalculateAngle(Aimpoint);
			Angle -= (G::Localplayer->GetAimPunchAngle() * 2);

			// shit hitchance, nah
			if constexpr (DEBUG_AIMBOT) L::Debug("hitchance");
			float hitchance = 0;
			/*if (i == 0)
				hitchance = CalculateHitchance(Angle, Aimpoint, player.pEntity, HITBOX);
			else*/
			hitchance = CalculatePsudoHitchance();
			if (hitchance < this->MinHitchance) continue;

			float Damage = 0;
			Damage = autowall->GetDamage(G::Localplayer, Aimpoint);

			L::Debug(("Autowal Damage: " + std::to_string(Damage)).c_str());

			// we just did autowall scan
			NumScan++;

			// damage check
			if (Damage < this->MinDamage) continue;

			// log that shot damage :D
			/*H::console.push_back(GetHitboxStr(HITBOX) + " " + std::to_string((int)Damage));
			while (H::console.size() > 10)
				H::console.erase(H::console.begin());*/

				// silent aim
			G::cmd->angViewAngle = QAngle(Angle.x, Angle.y);

			// FIRE POW POW POW!
			G::cmd->iButtons |= IN_ATTACK;

			// deal with lag comp
			G::cmd->iTickCount = TimeToTicks(record.SimulationTime /*+ GetLerp()*/) - 1; // magic math 0_0

			if (!cfg->Keybinds["Fake Duck"].boolean)
				*G::pSendpacket = true;

			if constexpr (DEBUG_AIMBOT) L::Debug("CapsuleOverlay");
			// show the shot :D
			//CapsuleOverlay(player.pEntity, Color(255, 0, 0, 255), 4, record.Matrix);

			/*std::string msg = "Shot at [" + (std::string)player.Info.szName + "] with hitchance [" + std::to_string(hitchance) + "]\n";
			ConsoleColorMsg(Color(0, 255, 0, 255), msg.c_str());*/

			Resolver::aim_shot_log shot;
			shot.userID = player_userid.first;
			shot.time = record.SimulationTime;// I::globalvars->flRealTime;
			shot.hitgroup = HitboxToHitgroup(HITBOX);
			shot.damage = Damage;
			shot.shootPos = G::Localplayer->GetEyePosition();
			shot.shootPoint = Aimpoint;
			shot.origin = record.Origin;
			shot.min_bone = min;
			shot.max_bone = max;
			shot.radius_bone = record.bones[HITBOX].flRadius;
			shot.safepoint = safepoint;
			resolver->AimbotShot(shot);

			// we found a good scan, shoot at it bitch
			goto END_AIM;
		}
	}

END_AIM:
	
	// for debugging
	if (NumScan > 0)
	{
		// calculate theoretical # of scans you can do per tick
		float intervalPerTick = I::globalvars->flIntervalPerTick;
		float avgScanTime = this->DeltaTime / NumScan;
		TheoreticalScans = intervalPerTick / avgScanTime;
	}
	
	return;
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