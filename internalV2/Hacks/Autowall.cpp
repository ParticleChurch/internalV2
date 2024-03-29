#include "../Include.hpp"

CAutoWall* autowall = new CAutoWall();

#define DEBUG_AUTOWALL false

float CAutoWall::GetDamage(Vector start, Entity* pLocal, const Vector& vecPoint, FireBulletData_t* pDataOut)
{
	if constexpr (DEBUG_AUTOWALL) L::Debug("GetDamage");

	const Vector vecPosition = start;

	// setup data
	FireBulletData_t data = { };
	data.vecPosition = vecPosition;
	data.vecDirection = (vecPoint - vecPosition).Normalized();

	if (Entity* pWeapon = pLocal->GetActiveWeapon(); pWeapon == nullptr || !SimulateFireBullet(pLocal, pWeapon, data))
		return -1.0f;

	if (pDataOut != nullptr)
		*pDataOut = data;

	return data.flCurrentDamage;
}

float CAutoWall::GetDamage(Entity* pLocal, const Vector& vecPoint, FireBulletData_t* pDataOut)
{
	if constexpr (DEBUG_AUTOWALL) L::Debug("GetDamage");

	const Vector vecPosition = pLocal->GetEyePosition();

	// setup data
	FireBulletData_t data = { };
	data.vecPosition = vecPosition;
	data.vecDirection = (vecPoint - vecPosition).Normalized();

	if (Entity* pWeapon = pLocal->GetActiveWeapon(); pWeapon == nullptr || !SimulateFireBullet(pLocal, pWeapon, data))
		return -1.0f;

	if (pDataOut != nullptr)
		*pDataOut = data;

	return data.flCurrentDamage;
}

void CAutoWall::ScaleDamage(const int iHitGroup, Entity* pEntity, const float flWeaponArmorRatio, const float flWeaponHeadShotMultiplier, float& flDamage)
{
	if constexpr (DEBUG_AUTOWALL) L::Debug("ScaleDamage");

	// @ida traceattack: server.dll @ 55 8B EC 83 E4 F8 81 EC C0 00 00 00 56 8B 75 08 57 8B F9 C6 44 24 13 01

	const bool bHeavyArmor = pEntity->HasHeavyArmor();

	static CConVar* mp_damage_scale_ct_head = I::convar->FindVar("mp_damage_scale_ct_head");
	static CConVar* mp_damage_scale_t_head = I::convar->FindVar("mp_damage_scale_t_head");

	static CConVar* mp_damage_scale_ct_body = I::convar->FindVar("mp_damage_scale_ct_body");
	static CConVar* mp_damage_scale_t_body = I::convar->FindVar("mp_damage_scale_t_body");

	float flHeadDamageScale = pEntity->GetTeam() == TEAM_CT ? mp_damage_scale_ct_head->GetFloat() : pEntity->GetTeam() == TEAM_TT ? mp_damage_scale_t_head->GetFloat() : 1.0f;
	const float flBodyDamageScale = pEntity->GetTeam() == TEAM_CT ? mp_damage_scale_ct_body->GetFloat() : pEntity->GetTeam() == TEAM_TT ? mp_damage_scale_t_body->GetFloat() : 1.0f;

	if (bHeavyArmor)
		flHeadDamageScale *= 0.5f;

	switch (iHitGroup)
	{
	case HITGROUP_HEAD:
		flDamage *= flWeaponHeadShotMultiplier * flHeadDamageScale;
		break;
	case HITGROUP_CHEST:
	case HITGROUP_LEFTARM:
	case HITGROUP_RIGHTARM:
	case HITGROUP_NECK:
		flDamage *= flBodyDamageScale;
		break;
	case HITGROUP_STOMACH:
		flDamage *= 1.25f * flBodyDamageScale;
		break;
	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:
		flDamage *= 0.75f * flBodyDamageScale;
		break;
	default:
		break;
	}

	if (pEntity->IsArmored(iHitGroup))
	{
		// @ida ontakedamage: server.dll @ 80 BF ? ? ? ? ? F3 0F 10 5C 24 ? F3 0F 10 35

		const int iArmor = pEntity->GetArmor();
		float flHeavyArmorBonus = 1.0f, flArmorBonus = 0.5f, flArmorRatio = flWeaponArmorRatio * 0.5f;

		if (bHeavyArmor)
		{
			flHeavyArmorBonus = 0.25f;
			flArmorBonus = 0.33f;
			flArmorRatio *= 0.20f;
		}

		float flDamageToHealth = flDamage * flArmorRatio;
		if (const float flDamageToArmor = (flDamage - flDamageToHealth) * (flHeavyArmorBonus * flArmorBonus); flDamageToArmor > static_cast<float>(iArmor))
			flDamageToHealth = flDamage - static_cast<float>(iArmor) / flArmorBonus;

		flDamage = flDamageToHealth;
	}
}

// @credits: https://github.com/perilouswithadollarsign/cstrike15_src/blob/master/game/shared/util_shared.cpp#L757
void CAutoWall::ClipTraceToPlayers(const Vector& vecAbsStart, const Vector& vecAbsEnd, const unsigned int fMask, ITraceFilter* pFilter, Trace_t* pTrace, const float flMinRange)
{
	// @ida util_cliptracetoplayers: client.dll @ E8 ? ? ? ? 0F 28 84 24 68 02 00 00

	Trace_t trace = { };
	float flSmallestFraction = pTrace->flFraction;

	const Ray_t ray(vecAbsStart, vecAbsEnd);

	for (int i = 1; i < I::globalvars->nMaxClients; i++)
	{
		Entity* pEntity = I::entitylist->GetClientEntity(i);

		if (pEntity == nullptr || !pEntity->IsAlive() || pEntity->IsDormant())
			continue;

		if (pFilter != nullptr && !pFilter->ShouldHitEntity((IHandleEntity*)pEntity, fMask))
			continue;

		const ICollideable* pCollideable = pEntity->GetCollideable();

		if (pCollideable == nullptr)
			continue;

		// get bounding box
		const Vector vecMin = pCollideable->OBBMins();
		const Vector vecMax = pCollideable->OBBMaxs();

		// calculate world space center
		const Vector vecCenter = (vecMax + vecMin) * 0.5f;
		const Vector vecPosition = vecCenter + pEntity->GetVecOrigin();

		const Vector vecTo = vecPosition - vecAbsStart;
		Vector vecDirection = vecAbsEnd - vecAbsStart;
		const float flLength = vecDirection.NormalizeInPlace();

		const float flRangeAlong = vecDirection.DotProduct(vecTo);
		float flRange = 0.0f;

		// calculate distance to ray
		if (flRangeAlong < 0.0f)
			// off start point
			flRange = -vecTo.Length();
		else if (flRangeAlong > flLength)
			// off end point
			flRange = -(vecPosition - vecAbsEnd).Length();
		else
			// within ray bounds
			flRange = (vecPosition - (vecDirection * flRangeAlong + vecAbsStart)).Length();

		constexpr float flMaxRange = 60.f;
		if (flRange < flMinRange || flRange > flMaxRange)
			continue;

		I::enginetrace->ClipRayToEntity(ray, fMask | CONTENTS_HITBOX, (IHandleEntity*)pEntity, &trace);

		if (trace.flFraction < flSmallestFraction)
		{
			// we shortened the ray - save off the trace
			*pTrace = trace;
			flSmallestFraction = trace.flFraction;
		}
	}
}

bool CAutoWall::TraceToExit(Trace_t& enterTrace, Trace_t& exitTrace, const Vector& vecPosition, const Vector& vecDirection, const Entity* pClipPlayer)
{
	if constexpr (DEBUG_AUTOWALL) L::Debug("TraceToExit");

	// @ida tracetoexit: client.dll @ 55 8B EC 83 EC 4C F3

	float flDistance = 0.0f;
	int iStartContents = 0;

	while (flDistance <= 90.0f)
	{
		// add extra distance to our ray
		flDistance += 4.0f;

		// multiply the direction vector to the distance so we go outwards, add our position to it
		Vector vecStart = vecPosition + vecDirection * flDistance;

		if (!iStartContents)
			iStartContents = I::enginetrace->GetPointContents(vecStart, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr);

		const int iCurrentContents = I::enginetrace->GetPointContents(vecStart, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr);

		if (!(iCurrentContents & MASK_SHOT_HULL) || (iCurrentContents & CONTENTS_HITBOX && iCurrentContents != iStartContents))
		{
			// setup our end position by deducting the direction by the extra added distance
			const Vector vecEnd = vecStart - (vecDirection * 4.0f);

			// trace ray to world
			Ray_t rayWorld(vecStart, vecEnd);
			I::enginetrace->TraceRay(rayWorld, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr, &exitTrace);

			if (static CConVar* sv_clip_penetration_traces_to_players = I::convar->FindVar("sv_clip_penetration_traces_to_players"); sv_clip_penetration_traces_to_players != nullptr && sv_clip_penetration_traces_to_players->GetBool())
			{
				CTraceFilter filter((IHandleEntity*)pClipPlayer);
				ClipTraceToPlayers(vecEnd, vecStart, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &exitTrace, -60.f);
			}

			// check if a hitbox is in-front of our enemy and if they are behind of a solid wall
			if (exitTrace.bStartSolid && exitTrace.surface.uFlags & SURF_HITBOX)
			{
				// trace ray to entity
				Ray_t ray(vecStart, vecPosition);
				CTraceFilter filter((IHandleEntity*)exitTrace.pHitEntity);

				I::enginetrace->TraceRay(ray, MASK_SHOT_HULL, &filter, &exitTrace);

				if (exitTrace.DidHit() && !exitTrace.bStartSolid)
				{
					vecStart = exitTrace.vecEnd;
					return true;
				}

				continue;
			}

			if (exitTrace.DidHit() && !exitTrace.bStartSolid)
			{
				if (enterTrace.pHitEntity->IsBreakable() && exitTrace.pHitEntity->IsBreakable())
					return true;

				if (enterTrace.surface.uFlags & SURF_NODRAW || (!(exitTrace.surface.uFlags & SURF_NODRAW) && exitTrace.plane.vecNormal.DotProduct(vecDirection) <= 1.0f))
				{
					const float flMultiplier = exitTrace.flFraction * 4.0f;
					vecStart -= vecDirection * flMultiplier;
					return true;
				}

				continue;
			}

			if (!exitTrace.DidHit() || exitTrace.bStartSolid)
			{
				if (enterTrace.pHitEntity != nullptr && enterTrace.pHitEntity->GetIndex() != 0 && enterTrace.pHitEntity->IsBreakable())
				{
					// did hit breakable non world entity
					exitTrace = enterTrace;
					exitTrace.vecEnd = vecStart + vecDirection;
					return true;
				}

				continue;
			}
		}
	}

	return false;
}

bool CAutoWall::HandleBulletPenetration(Entity* pLocal, const CCSWeaponData* pWeaponData, const surfacedata_t* pEnterSurfaceData, FireBulletData_t& data)
{
	if constexpr (DEBUG_AUTOWALL) L::Debug("HandleBulletPenetration");

	// @ida handlebulletpenetration: client.dll @ E8 ? ? ? ? 83 C4 40 84 C0

	static CConVar* ff_damage_reduction_bullets = I::convar->FindVar("ff_damage_reduction_bullets");
	static CConVar* ff_damage_bullet_penetration = I::convar->FindVar("ff_damage_bullet_penetration");

	const float flReductionDamage = ff_damage_reduction_bullets->GetFloat();
	const float flPenetrateDamage = ff_damage_bullet_penetration->GetFloat();

	const MaterialHandle_t hEnterMaterial = pEnterSurfaceData->game.hMaterial;
	const float flEnterPenetrationModifier = pEnterSurfaceData->game.flPenetrationModifier;
	const bool bIsSolidSurf = ((data.enterTrace.iContents >> 3) & CONTENTS_SOLID);
	const bool bIsLightSurf = ((data.enterTrace.surface.uFlags >> 7) & SURF_LIGHT);

	Trace_t exitTrace = { };
	if (data.iPenetrateCount <= 0 ||
		(!data.iPenetrateCount && !bIsLightSurf && !bIsSolidSurf && hEnterMaterial != CHAR_TEX_GRATE && hEnterMaterial != CHAR_TEX_GLASS) ||
		(pWeaponData->flPenetration <= 0.0f) ||
		(!TraceToExit(data.enterTrace, exitTrace, data.enterTrace.vecEnd, data.vecDirection, pLocal) && !(I::enginetrace->GetPointContents(data.enterTrace.vecEnd, MASK_SHOT_HULL, nullptr) & MASK_SHOT_HULL)))
		return false;

	const surfacedata_t* pExitSurfaceData = I::physicsprops->GetSurfaceData(exitTrace.surface.nSurfaceProps);
	const MaterialHandle_t hExitMaterial = pExitSurfaceData->game.hMaterial;
	const float flExitPenetrationModifier = pExitSurfaceData->game.flPenetrationModifier;

	float flDamageLostModifier = 0.16f;
	float flPenetrationModifier = 0.0f;

	if (hEnterMaterial == CHAR_TEX_GRATE || hEnterMaterial == CHAR_TEX_GLASS)
	{
		flDamageLostModifier = 0.05f;
		flPenetrationModifier = 3.0f;
	}
	else if (bIsSolidSurf || bIsLightSurf)
	{
		flDamageLostModifier = 0.16f;
		flPenetrationModifier = 1.0f;
	}
	else if (hEnterMaterial == CHAR_TEX_FLESH && (pLocal->GetTeam() == data.enterTrace.pHitEntity->GetTeam() && flReductionDamage == 0.0f))
	{
		if (flPenetrateDamage == 0.0f)
			return false;

		// shoot through teammates
		flDamageLostModifier = 0.16f;
		flPenetrationModifier = flPenetrateDamage;
	}
	else
	{
		flDamageLostModifier = 0.16f;
		flPenetrationModifier = (flEnterPenetrationModifier + flExitPenetrationModifier) * 0.5f;
	}

	if (hEnterMaterial == hExitMaterial)
	{
		if (hExitMaterial == CHAR_TEX_CARDBOARD || hExitMaterial == CHAR_TEX_WOOD)
			flPenetrationModifier = 3.0f;
		else if (hExitMaterial == CHAR_TEX_PLASTIC)
			flPenetrationModifier = 2.0f;
	}

	const float flTraceDistance = (exitTrace.vecEnd - data.enterTrace.vecEnd).LengthSqr();

	// penetration modifier
	const float flModifier = max(0.0f, 1.0f / flPenetrationModifier);

	// this calculates how much damage we've lost depending on thickness of the wall, our penetration, damage, and the modifiers set earlier
	const float flLostDamage = (data.flCurrentDamage * flDamageLostModifier + max(0.0f, 3.75f / pWeaponData->flPenetration) * (flModifier * 3.0f)) + ((flModifier * flTraceDistance) / 24.0f);

	// did we loose too much damage?
	if (flLostDamage > data.flCurrentDamage)
		return false;

	// we can't use any of the damage that we've lost
	if (flLostDamage > 0.0f)
		data.flCurrentDamage -= flLostDamage;

	// do we still have enough damage to deal?
	if (data.flCurrentDamage < 1.0f)
		return false;

	data.vecPosition = exitTrace.vecEnd;
	--data.iPenetrateCount;
	return true;
}

bool CAutoWall::SimulateFireBullet(Entity* pLocal, Entity* pWeapon, FireBulletData_t& data)
{
	
	if constexpr (DEBUG_AUTOWALL) L::Debug("SimulateFireBullet");

	// @ida firebullet: client.dll @ 55 8B EC 83 E4 F0 81 EC ? ? ? ? F3 0F 7E

	CCSWeaponData* pWeaponData = I::weaponsystem->GetWeaponData(pWeapon->GetItemDefinitionIndex());

	if (pWeaponData == nullptr)
		return false;

	float flMaxRange = pWeaponData->flRange;

	// the total number of surfaces any bullet can penetrate in a single flight is capped at 4
	data.iPenetrateCount = 4;
	// set our current damage to what our gun's initial damage reports it will do
	data.flCurrentDamage = static_cast<float>(pWeaponData->iDamage);

	float flTraceLenght = 0.0f;
	CTraceFilter filter((IHandleEntity*)pLocal);

	while (data.iPenetrateCount > 0 && data.flCurrentDamage >= 1.0f)
	{
		// max bullet range
		flMaxRange -= flTraceLenght;

		// end position of bullet
		const Vector vecEnd = data.vecPosition + data.vecDirection * flMaxRange;

		Ray_t ray(data.vecPosition, vecEnd);
		I::enginetrace->TraceRay(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &data.enterTrace);

		// check for player hitboxes extending outside their collision bounds
		ClipTraceToPlayers(data.vecPosition, vecEnd + data.vecDirection * 40.0f, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &data.enterTrace);

		const surfacedata_t* pEnterSurfaceData = I::physicsprops->GetSurfaceData(data.enterTrace.surface.nSurfaceProps);
		const float flEnterPenetrationModifier = pEnterSurfaceData->game.flPenetrationModifier;

		// we didn't hit anything, stop tracing shoot
		if (data.enterTrace.flFraction == 1.0f)
		{
			return false;

			// normally bad, but if we get to the point then its a win so... (ONLY FOR BACKTRACK)
			/*ScaleDamage(data.enterTrace.iHitGroup, data.enterTrace.pHitEntity, pWeaponData->flArmorRatio, pWeaponData->flHeadShotMultiplier, data.flCurrentDamage);
			return true;*/
		}

		// calculate the damage based on the distance the bullet traveled
		flTraceLenght += data.enterTrace.flFraction * flMaxRange;
		data.flCurrentDamage *= std::powf(pWeaponData->flRangeModifier, flTraceLenght / MAX_DAMAGE);

		// check is actually can shoot through
		if (flTraceLenght > 3000.f || flEnterPenetrationModifier < 0.1f)
			break;

		// check is can do damage
		if (data.enterTrace.iHitGroup != HITGROUP_GENERIC && data.enterTrace.iHitGroup != HITGROUP_GEAR && G::LocalplayerTeam != data.enterTrace.pHitEntity->GetTeam())
		{
			// we got target - scale damage
			ScaleDamage(data.enterTrace.iHitGroup, data.enterTrace.pHitEntity, pWeaponData->flArmorRatio, pWeaponData->flHeadShotMultiplier, data.flCurrentDamage);
			return true;
		}

		// calling handlebulletpenetration here reduces our penetration �ounter, and if it returns true, we can't shoot through it
		if (!HandleBulletPenetration(pLocal, pWeaponData, pEnterSurfaceData, data))
			break;
	}

	return false;
}
