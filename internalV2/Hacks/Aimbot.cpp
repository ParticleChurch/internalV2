#include "../Include.hpp"

Aimbot* aimbot = new Aimbot();

#define DEBUG_AIMBOT true

void Aimbot::UpdateHitboxes()
{
	if constexpr (DEBUG_AIMBOT) L::Debug("UpdateHitboxes");

	// only update everytime we change our choice
	static int LastPriorityChoice = -1;
	if (cfg->aimbot.Priority != LastPriorityChoice)
	{
		LastPriorityChoice = cfg->aimbot.Priority;
		// clear the list
		this->priority.clear();
		this->priority.resize(0);

		// send to priority the relavent hitboxes
		switch (cfg->aimbot.Priority)
		{
		case 0:
			break;
		case 1:
			this->priority.push_back(HITBOX_HEAD);
			break;
		case 2:
			this->priority.push_back(HITBOX_NECK);
			break;
		case 3:
			this->priority.push_back(HITBOX_PELVIS);
			break;
		case 4:
			this->priority.push_back(HITBOX_STOMACH);
			break;
		case 5:
			this->priority.push_back(HITBOX_THORAX);
			break;
		case 6:
			this->priority.push_back(HITBOX_CHEST);
			break;
		case 7:
			this->priority.push_back(HITBOX_UPPER_CHEST);
			break;
		case 8:
			this->priority.push_back(HITBOX_LEFT_THIGH);
			this->priority.push_back(HITBOX_RIGHT_THIGH);
			break;
		case 9:
			this->priority.push_back(HITBOX_LEFT_CALF);
			this->priority.push_back(HITBOX_RIGHT_CALF);
			break;
		case 10:
			this->priority.push_back(HITBOX_LEFT_FOOT);
			this->priority.push_back(HITBOX_RIGHT_FOOT);
			break;
		case 11:
			this->priority.push_back(HITBOX_LEFT_HAND);
			this->priority.push_back(HITBOX_RIGHT_HAND);
			break;
		case 12:
			this->priority.push_back(HITBOX_RIGHT_UPPER_ARM);
			this->priority.push_back(HITBOX_LEFT_UPPER_ARM);
			break;
		case 13:
			this->priority.push_back(HITBOX_RIGHT_FOREARM);
			this->priority.push_back(HITBOX_LEFT_FOREARM);
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

	this->hitboxes.clear();
	this->hitboxes.resize(0);

	if (cfg->aimbot.Hitboxes[0])
		this->hitboxes.push_back(HITBOX_HEAD);
	if (cfg->aimbot.Hitboxes[1])
		this->hitboxes.push_back(HITBOX_NECK);
	if (cfg->aimbot.Hitboxes[2])
		this->hitboxes.push_back(HITBOX_PELVIS);
	if (cfg->aimbot.Hitboxes[3])
		this->hitboxes.push_back(HITBOX_STOMACH);
	if (cfg->aimbot.Hitboxes[4])
		this->hitboxes.push_back(HITBOX_THORAX);
	if (cfg->aimbot.Hitboxes[5])
		this->hitboxes.push_back(HITBOX_CHEST);
	if (cfg->aimbot.Hitboxes[6])
		this->hitboxes.push_back(HITBOX_UPPER_CHEST);
	if (cfg->aimbot.Hitboxes[7])
	{
		this->hitboxes.push_back(HITBOX_LEFT_THIGH);
		this->hitboxes.push_back(HITBOX_RIGHT_THIGH);
	}
	if (cfg->aimbot.Hitboxes[8])
	{
		this->hitboxes.push_back(HITBOX_LEFT_CALF);
		this->hitboxes.push_back(HITBOX_RIGHT_CALF);
	}
	if (cfg->aimbot.Hitboxes[9])
	{
		this->hitboxes.push_back(HITBOX_LEFT_FOOT);
		this->hitboxes.push_back(HITBOX_RIGHT_FOOT);
	}
	if (cfg->aimbot.Hitboxes[10])
	{
		this->hitboxes.push_back(HITBOX_LEFT_HAND);
		this->hitboxes.push_back(HITBOX_RIGHT_HAND);
	}
	if (cfg->aimbot.Hitboxes[11])
	{
		this->hitboxes.push_back(HITBOX_LEFT_UPPER_ARM);
		this->hitboxes.push_back(HITBOX_RIGHT_UPPER_ARM);
	}
	if (cfg->aimbot.Hitboxes[12])
	{
		this->hitboxes.push_back(HITBOX_LEFT_FOREARM);
		this->hitboxes.push_back(HITBOX_RIGHT_FOREARM);
	}
}

float Aimbot::CalculateHitchance(Vector vangles, const Vector& point, Entity* player, int hbox)
{
	if constexpr (DEBUG_AIMBOT) L::Debug("CalculateHitchance");

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
	if constexpr (DEBUG_AIMBOT) L::Debug("CalculatePsudoHitchance");

	if (!G::LocalplayerWeapon)
		return 0;
	return 0.f;
}

bool Aimbot::ValidPlayer(LagComp::Player player)
{
	if constexpr (DEBUG_AIMBOT) L::Debug("ValidPlayer");

	return false;
}

void Aimbot::GetPlayers(std::vector<std::pair<int, float>>& values)
{
	if constexpr (DEBUG_AIMBOT) L::Debug("GetPlayers");

	//update every .1 sec
	static float LastTimeUpdate = I::globalvars->flCurrentTime;
	if (fabsf(I::globalvars->flCurrentTime - LastTimeUpdate) < 0.1)
		return;
	LastTimeUpdate = I::globalvars->flCurrentTime;

	values.clear();
	values.resize(0);

	PlayerInfo_t info;

	Vector localorigin = G::Localplayer->GetVecOrigin();
	for (auto a : lagcomp->PlayerList)
	{
		if (a.second.Index == G::LocalplayerIndex)
			continue;
		if (a.second.Team == G::LocalplayerTeam)
			continue;
		if (!a.second.pEntity)
			continue;
		if (!(a.second.pEntity->IsAlive() > 0))
			continue;
		if (!I::engine->GetPlayerInfo(a.second.Index, &info))
			continue;
		if (a.second.Dormant)
			continue;
		if (!a.second.pEntity->IsPlayer()) continue;
		values.push_back(std::pair(a.first, (100 - a.second.pEntity->GetHealth()) * 8 + 80000 / (a.second.pEntity->GetVecOrigin() - localorigin).Length2D()));
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

float Aimbot::GetPointScale(int Hitbox)
{
	if constexpr (DEBUG_AIMBOT) L::Debug("GetPointScale");

	switch (Hitbox)
	{
	case HITBOX_HEAD:
		return this->HeadScale;
		break;
	case HITBOX_CHEST:
	case HITBOX_THORAX:
	case HITBOX_UPPER_CHEST:
	case HITBOX_PELVIS:
	case HITBOX_STOMACH:
		return this->BodyScale;
	default:
		return this->LimbScale;
	}
	// just in case a sneaky boi gets by (it wont but whatev)
	return this->LimbScale;
}

bool Aimbot::ScanPlayers()
{
	if constexpr (DEBUG_AIMBOT) L::Debug("ScanPlayers");

	// reset the number of scans
	NumScan = 0;

	// reset backtrackshot bool to zero
	BacktrackShot = false;

	// reset the max hitchance found
	MaxHitchance = 0.f;

	// reset the max damage found
	MaxDamage = 0.f;

	// scan players
	for (auto& a : this->players)
	{
		/*if (ScanPlayerBacktrack(a.first))
		{
			BacktrackShot = true;
			return true;
		}*/

		if (ScanPlayer(a.first))
			return true;
	}
	return false;
}

bool Aimbot::ScanPlayer(int UserID)
{
	if constexpr (DEBUG_AIMBOT) L::Debug("ScanPlayer UserID");

	Entity* ent = lagcomp->PlayerList[UserID].pEntity;

	if (!ent) return false;

	if (!(ent->GetHealth() > 0)) return false;

	if (ent->IsDormant()) return false;

	Model_t* model = ent->GetModel();
	if (!model) return false;

	studiohdr_t* StudioModel = I::modelinfo->GetStudioModel(model);
	if (!StudioModel) return false; //if cant get the model

	// if bad setupbones
	if (!ent->SetupBones(AimMatrix, 128, 0x100, ent->GetSimulationTime(), false)) return false;

	// PRIORITY SCAN FIRST
	if (!priority.empty())
	{
		for (auto HITBOX : this->priority)
		{
			// RETURN the moment we have scanned too much
			if (NumScan > MaxScan) return false;

			mstudiobbox_t* StudioBox = StudioModel->GetHitboxSet(0)->GetHitbox(HITBOX);
			if (!StudioBox) continue;	//if cant get the hitbox...

			int HitGroup = HitboxToHitgroup(HITBOX);
			float radius = StudioBox->flRadius * GetPointScale(HITBOX);

			Vector min = StudioBox->vecBBMin.Transform(AimMatrix[StudioBox->iBone]);
			Vector max = StudioBox->vecBBMax.Transform(AimMatrix[StudioBox->iBone]);

			Vector point = (min + max) / 2.f;

			//special head multipoint
			if (HITBOX == HITBOX_HEAD && GetPointScale(HITBOX) > 0.01f)
			{
				// go for upper echelons of hittin head
				point = min.z > max.z ? min : max;

				// go for upper echelons of safepoint shittery
				Vector safepoint;
				bool FoundSafepoint = GetSafepoint(lagcomp->PlayerList[UserID].pEntity, UserID, point, StudioBox->flRadius * 0.9, safepoint);
				if (FoundSafepoint)
				{
					point = safepoint;
					/*debug->points.clear();
					debug->points.resize(0);
					debug->points2.push_back(point);*/
				}


				// we scan left, one tick, then scan right the next, and then the top tick, then repeat
				// this is to hopefully de-lag, though we can potentially do bad by 1 tick.
				// this is counter by the fact that, because we aren't lagging, we will be most up to date
				// maybe I'll add 2 implementations, with the user deciding which one to use

				if (!FoundSafepoint)
				{
					bool OnGround = lagcomp->PlayerList[UserID].pEntity->GetFlags() & FL_ONGROUND;
					if (OnGround)
					{
						switch (G::cmd->iTickCount % 2)
						{
						case 0: // target top left
							point = lagcomp->PlayerList[UserID].pEntity->GetTopLeft(point, radius, G::Localplayer);
							break;
						case 1: // target top right
							point = lagcomp->PlayerList[UserID].pEntity->GetTopRight(point, radius, G::Localplayer);
							break;
						}
					}
					else
					{
						// they are in air -> check more points
						switch (G::cmd->iTickCount % 4)
						{
						case 0: // target bottom left
							// get the LOWEST point
							point = min.z > max.z ? max : min;
							point = lagcomp->PlayerList[UserID].pEntity->GetBottomLeft(point, radius, G::Localplayer);
							break;
						case 1: // target bottom right
							// get the LOWEST point
							point = min.z > max.z ? max : min;
							point = lagcomp->PlayerList[UserID].pEntity->GetBottomRight(point, radius, G::Localplayer);
							break;
						case 2: // target top left
							point = lagcomp->PlayerList[UserID].pEntity->GetTopLeft(point, radius, G::Localplayer);
							break;
						case 3: // target top right
							point = lagcomp->PlayerList[UserID].pEntity->GetTopRight(point, radius, G::Localplayer);
							break;
						}
					}
				}
			}
			// utter shittery
			else if ((HITBOX == HITBOX_STOMACH || HITBOX == HITBOX_PELVIS || HITBOX == HITBOX_UPPER_CHEST || HITBOX == HITBOX_THORAX) && GetPointScale(HITBOX))
			{
				// we scan left, one tick, then scan right the next, and then the top tick, then repeat
				// this is to hopefully de-lag, though we can potentially do bad by 2 tick.
				// this is counter by the fact that, because we aren't lagging, we will be most up to date
				// maybe I'll add 2 implementations, with the user deciding which one to use
				switch (G::cmd->iTickCount % 2)
				{
				case 0: // target left
					point = lagcomp->PlayerList[UserID].pEntity->GetLeft(point, radius, G::Localplayer);
					break;
				case 1: // target right
					point = lagcomp->PlayerList[UserID].pEntity->GetRight(point, radius, G::Localplayer);
					break;
				}
			}

			// Calc Angle
			Vector ang = Calc::CalculateAngle(point);

			// Calc Hitchance
			float hitchance = CalculateHitchance(ang, point, lagcomp->PlayerList[UserID].pEntity, HITBOX);

			// make sure to update the hitchance
			if (MaxHitchance < hitchance)
				MaxHitchance = hitchance;

			// if it is zero, just continue
			if (!(hitchance > 0.f)) continue;

			// scan
			FireBulletData_t data = { };

			float damage = autowall->GetDamage(G::Localplayer, point, &data);
			NumScan++;

			if (damage > MaxDamage)
				MaxDamage = damage;

			// if possible to do damage with proper hitchance
			if (hitchance < this->MinHitchance / 100.f) continue;

			// if good enough damage
			if (damage >= this->MinDamage || damage >= lagcomp->PlayerList[UserID].Health)
			{
				AimUserID = UserID;
				//AimTickCount = TimeToTicks(lagcomp->PlayerList[UserID].records.front().SimulationTime + GetLerp());
				AimTickCount = TimeToTicks(lagcomp->PlayerList[UserID].pEntity->GetSimulationTime() /*+ GetLerp()*/);
				AimPoint = point;

				return true;
			}
		}
	}

	// REGULAR SCAN NEXT
	for (auto HITBOX : this->hitboxes)
	{
		// RETURN the moment we have scanned too much
		if (NumScan > MaxScan) return false;

		// SKIP if in priority hitboxes
		if (!priority.empty())
		{
			bool DoSkip = false;
			for (auto& a : this->priority)
			{
				if (a == HITBOX)
					DoSkip = true;
			}
			if (DoSkip)
				continue;
		}

		mstudiobbox_t* StudioBox = StudioModel->GetHitboxSet(0)->GetHitbox(HITBOX);
		if (!StudioBox) continue;	//if cant get the hitbox...
		int HitGroup = HitboxToHitgroup(HITBOX);
		float radius = StudioBox->flRadius * GetPointScale(HITBOX);

		Vector min = StudioBox->vecBBMin.Transform(AimMatrix[StudioBox->iBone]);
		Vector max = StudioBox->vecBBMax.Transform(AimMatrix[StudioBox->iBone]);

		Vector point = (min + max) / 2.f;

		//special head multipoint
		if (HITBOX == HITBOX_HEAD && GetPointScale(HITBOX) > 0.01f)
		{
			// go for upper echelons of hittin head
			point = min.z > max.z ? min : max;

			// we scan left, one tick, then scan right the next, and then the top tick, then repeat
			// this is to hopefully de-lag, though we can potentially do bad by 1 tick.
			// this is counter by the fact that, because we aren't lagging, we will be most up to date
			// maybe I'll add 2 implementations, with the user deciding which one to use

			switch (G::cmd->iTickCount % 2)
			{
			case 0: // target top left
				point = lagcomp->PlayerList[UserID].pEntity->GetTopLeft(point, radius, G::Localplayer);
				break;
			case 1: // target top right
				point = lagcomp->PlayerList[UserID].pEntity->GetTopRight(point, radius, G::Localplayer);
				break;
			}
		}
		// utter shittery
		else if ((HITBOX == HITBOX_STOMACH || HITBOX == HITBOX_PELVIS || HITBOX == HITBOX_UPPER_CHEST || HITBOX == HITBOX_THORAX) && GetPointScale(HITBOX))
		{
			// we scan left, one tick, then scan right the next, and then the top tick, then repeat
			// this is to hopefully de-lag, though we can potentially do bad by 2 tick.
			// this is counter by the fact that, because we aren't lagging, we will be most up to date
			// maybe I'll add 2 implementations, with the user deciding which one to use
			switch (G::cmd->iTickCount % 2)
			{
			case 0: // target left
				point = lagcomp->PlayerList[UserID].pEntity->GetLeft(point, radius, G::Localplayer);
				break;
			case 1: // target right
				point = lagcomp->PlayerList[UserID].pEntity->GetRight(point, radius, G::Localplayer);
				break;
			}
		}

		// Calc Angle
		Vector ang = Calc::CalculateAngle(point);

		// Calc Hitchance
		float hitchance = CalculateHitchance(ang, point, lagcomp->PlayerList[UserID].pEntity, HITBOX);

		// if possible to do damage
		if (hitchance < this->MinHitchance / 100.f) continue;

		// scan
		FireBulletData_t data = { };

		float damage = autowall->GetDamage(G::Localplayer, point, &data);
		NumScan++;

		// if good enough damage
		if (damage >= this->MinDamage)
		{
			AimUserID = UserID;
			//AimTickCount = TimeToTicks(lagcomp->PlayerList[UserID].records.front().SimulationTime + GetLerp());
			AimTickCount = TimeToTicks(lagcomp->PlayerList[UserID].pEntity->GetSimulationTime() /*+ GetLerp()*/);

			AimPoint = point;

			return true;
		}
	}
	return false;
}

bool Aimbot::ScanPlayerBacktrack(int UserID)
{
	if constexpr (DEBUG_AIMBOT) L::Debug("ScanPlayerBacktrack");

	return false;
}


//mp_roundtime_defuse 60;mp_roundtime_hostage 60;mp_roundtime 60;mp_restartgame 1
//mp_buy_anywhere 1;mp_buytime 60000

Vector RotatePoint(Vector pos, Vector orig, float ang)
{
	if constexpr (DEBUG_AIMBOT) L::Debug("RotatePoint");

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

// need.
// 1. AA
// 2. third person

bool Aimbot::GetSafepoint(Entity* ent, int UserID, Vector point, float radius, Vector& safepoint)
{
	if constexpr (DEBUG_AIMBOT) L::Debug("GetSafepoint");

	/*if (resolver->PlayerInfo.find(UserID) == resolver->PlayerInfo.end())
		return false;*/

		// USE OUTER POINTS to detect safepoints (mainly for better max_desync) safepointing
	Vector origin = ent->GetEyePosition(); origin.z = point.z;
	float dist = (point - origin).VecLength();

	Vector r1 = RotatePoint(point, origin, 180);
	Vector r2 = RotatePoint(point, origin, 180 + ent->GetMaxDesyncDelta());
	Vector r3 = RotatePoint(point, origin, 180 - ent->GetMaxDesyncDelta());

	std::vector<Vector> pts = { r1, r2, r3 };

	return FoundSafepoint(G::Localplayer->GetEyePosition(), pts, radius, safepoint);
}

void Aimbot::Rage()
{
	if constexpr (DEBUG_AIMBOT) L::Debug("Rage");

	//movement->AimReqAutostop = false;

	// if the player can SOON shoot a weapon
	if (/*cfg->other.AutoSlow*/ false)
	{
		Entity* weap = G::LocalplayerWeapon;
		if (!weap) return;
		float ServerTime = G::Localplayer->GetServerTime();

		// 200ms is autostop time, will add to autoslow :D
		if (G::Localplayer->GetNextAttack() - ServerTime > 0.2) return;
		if (weap->GetNextPrimaryAttack() - ServerTime > 0.2) return;
	}
	// If the player can't shoot a weapon... 
	else if (!G::Localplayer->CanShoot()) return;

	// if no ammo in weapon
	if (!(G::Localplayer->GetAmmo() > 0)) return;

	// don't bother if zeus or knife :D
	/*CBaseClient* clientclass = G::LocalplayerWeapon->GetClientClass();
	if (clientclass->nClassID == 0 || clientclass->nClassID == 40)
		return;*/

	auto start = std::chrono::steady_clock::now();

	// update the hitboxes
	UpdateHitboxes();

	// Get players (sorts)
	GetPlayers(this->players);

	// if we found a good player / target
	if (ScanPlayers() && G::Localplayer->CanShoot())
	{

		Vector Angle = Calc::CalculateAngle(this->AimPoint);
		Angle -= (G::Localplayer->GetAimPunchAngle() * 2);

		// silent aim
		G::cmd->angViewAngle = QAngle(Angle.x, Angle.y);

		// FIRE POW POW POW!
		G::cmd->iButtons |= IN_ATTACK;
		G::cmd->iTickCount = AimTickCount;

		// only force send if not fakeducking... (as that will break fd)
		/*if (G::pSendPacket)
			*G::pSendPacket = true;*/
	}

	auto end = std::chrono::steady_clock::now();

	auto diff = end - start;

	this->DeltaTime = std::chrono::duration <double, std::milli>(diff).count() / 1000.f;

	// linear based optimization (should be good enough)
	// -> max will be number scanned + 20
	// -> min will be 20

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

void Aimbot::Run()
{
	if constexpr (DEBUG_AIMBOT) L::Debug("Run");

	if (!cfg->Keybinds["Aimbot"].boolean) return;

	// update the aimbot values
	MinHitchance = cfg->aimbot.Hitchance;
	MinDamage = cfg->aimbot.MinDamage;
	HeadScale = cfg->aimbot.HeadPointScale / 100.f;
	BodyScale = cfg->aimbot.BodyPointScale / 100.f;
	LimbScale = cfg->aimbot.LimbPointScale / 100.f;

	Rage();
}

void Aimbot::RenderStats()
{
	if constexpr (DEBUG_AIMBOT) L::Debug("RenderStats");

	/*if (!cfg->performance.ShowConsole) return;*/

	ImGui::Begin("Aimbot stats");

	ImGui::Text(("Max Damage		: " + std::to_string(MaxDamage)).c_str());
	ImGui::Text(("Max Hitchance		: " + std::to_string(MaxHitchance)).c_str());

	ImGui::Text(("Max Scan		: " + std::to_string(MaxScan)).c_str());
	ImGui::Text(("Delta	Time	: " + std::to_string(DeltaTime)).c_str());
	ImGui::Text(("Alotted Time	: " + std::to_string(I::globalvars->flIntervalPerTick / 2.f)).c_str());
	if (ScanTimeBad)
		ImGui::Text("BAD SCAN TIME");

	ImGui::Separator();

	ImGui::Text("Priority");
	for (auto hitbox : this->priority)
		ImGui::Text(GetHitboxStr(hitbox).c_str());

	ImGui::Separator();

	ImGui::Text("Hitboxes");
	for (auto hitbox : this->hitboxes)
		ImGui::Text(GetHitboxStr(hitbox).c_str());


	ImGui::End();
}

/*
You can do a maximum of 500 hp damage to your teammates before you get kicked from the server.
However, overkill damage is also counted.
For example, if you kill a teammate with a head shot using an AWP, then it gets counted as 400+ damage rather than 100.
You can also get banned if you kill 3 teammates in the match.
*/