#include "../Include.hpp"

Triggerbot* triggerbot = new Triggerbot();

static auto fromAngle(const Vector& angle) noexcept
{
    return Vector{ std::cosf(DEG2RAD(angle.x)) * std::cosf(DEG2RAD(angle.y)),
                   std::cosf(DEG2RAD(angle.x)) * std::sinf(DEG2RAD(angle.y)),
                  -std::sinf(DEG2RAD(angle.x)) };
}

float Triggerbot::CalculatePsudoHitchance()
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

void Triggerbot::GetHitboxes()
{
	// update every second 
	static float oldTime = I::globalvars->flCurrentTime;
	if (fabsf(oldTime - I::globalvars->flCurrentTime) < 1)
		return;

	oldTime = I::globalvars->flCurrentTime;

	this->Hitboxes.clear();
	this->Hitboxes.resize(0);

	if (cfg->triggerbot.Hitboxes[0])
		this->Hitboxes.push_back(HITBOX_HEAD);
	if (cfg->triggerbot.Hitboxes[1])
		this->Hitboxes.push_back(HITBOX_NECK);
	if (cfg->triggerbot.Hitboxes[2])
		this->Hitboxes.push_back(HITBOX_PELVIS);
	if (cfg->triggerbot.Hitboxes[3])
		this->Hitboxes.push_back(HITBOX_STOMACH);
	if (cfg->triggerbot.Hitboxes[4])
		this->Hitboxes.push_back(HITBOX_THORAX);
	if (cfg->triggerbot.Hitboxes[5])
		this->Hitboxes.push_back(HITBOX_CHEST);
	if (cfg->triggerbot.Hitboxes[6])
		this->Hitboxes.push_back(HITBOX_UPPER_CHEST);
	if (cfg->triggerbot.Hitboxes[7])
	{
		this->Hitboxes.push_back(HITBOX_LEFT_THIGH);
		this->Hitboxes.push_back(HITBOX_RIGHT_THIGH);
	}
	if (cfg->triggerbot.Hitboxes[8])
	{
		this->Hitboxes.push_back(HITBOX_LEFT_CALF);
		this->Hitboxes.push_back(HITBOX_RIGHT_CALF);
	}
	if (cfg->triggerbot.Hitboxes[9])
	{
		this->Hitboxes.push_back(HITBOX_LEFT_FOOT);
		this->Hitboxes.push_back(HITBOX_RIGHT_FOOT);
	}
	if (cfg->triggerbot.Hitboxes[10])
	{
		this->Hitboxes.push_back(HITBOX_LEFT_HAND);
		this->Hitboxes.push_back(HITBOX_RIGHT_HAND);
	}
	if (cfg->triggerbot.Hitboxes[11])
	{
		this->Hitboxes.push_back(HITBOX_LEFT_UPPER_ARM);
		this->Hitboxes.push_back(HITBOX_RIGHT_UPPER_ARM);
	}
	if (cfg->triggerbot.Hitboxes[12])
	{
		this->Hitboxes.push_back(HITBOX_LEFT_FOREARM);
		this->Hitboxes.push_back(HITBOX_RIGHT_FOREARM);
	}
}

void Triggerbot::Run()
{
    H::console.clear();
    H::console.resize(0);
    H::console.push_back("TRIGGER");

    H::console.push_back("valid lastContact stuff");

    if (!cfg->Keybinds["Triggerbot"].boolean)
        return;

	if (!G::LocalplayerWeapondata) return;

	GetHitboxes();

	if (CalculatePsudoHitchance() < cfg->triggerbot.Hitchance / 100.f)
		return;

	H::console.push_back("valid hitchance");

    const auto startPos = G::Localplayer->GetEyePosition();
    const auto endPos = startPos + fromAngle(Vector(G::StartAngle.x, G::StartAngle.y) + G::Localplayer->GetAimPunchAngle()) * G::LocalplayerWeapondata->flRange;

	// deal with hitbox
	/*Trace_t trace;
	Ray_t ray = { startPos, endPos };
	I::enginetrace->TraceRay(ray, MASK_SHOT, (ITraceFilter*)G::Localplayer, &trace);
	if (!trace.pHitEntity) return;
	if (trace.pHitEntity->GetTeam() == G::LocalplayerTeam) return;
	bool bad_hitbox = true;
	for (auto a : this->Hitboxes)
	{
		if (HitboxToHitgroup(a) == trace.iHitGroup)
			bad_hitbox = false;
	}
	if (bad_hitbox) return;
	H::console.push_back("valid hitbox");*/

	// deal with damage
    float damage = autowall->GetDamage(G::Localplayer, endPos);
    if (damage < cfg->triggerbot.MinDamage)
        return;

    H::console.push_back("enemy player... ATTACK");

    //____________________________________________________________________________________________________
    /*if (trace.iHitGroup != cfg.hitgroup)
        return;*/
    
    G::cmd->iButtons |= IN_ATTACK;

}
