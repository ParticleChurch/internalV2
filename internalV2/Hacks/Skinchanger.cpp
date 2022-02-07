#include "../Include.hpp"

Skinchanger* skins = new Skinchanger();

void Skinchanger::Init()
{
	static bool doOnce = true;
	if (doOnce)
	{
		doOnce = false;
		EconomyItemCfg item;
		item.nFallbackPaintKit = 624;
		item.nFallbackSeed = 0;
		item.nFallbackStatTrak = 69;
		item.iEntityQuality = 4;
		item.szCustomName = (char*)"p100";
		item.flFallbackWear = 0.00001f;
		g_SkinChangerCfg.insert(std::pair(40, item));
	}
}

bool Skinchanger::ApplyCustomSkin(Entity* pWeapon)
{
	if (!pWeapon)
		return false;

	// Get the weapons item definition index.
	int nWeaponIndex = pWeapon->GetItemDefinitionIndex();

	// Check if this weapon has a valid override defined.
	if (g_SkinChangerCfg.find(nWeaponIndex) == g_SkinChangerCfg.end())
		return false;

	// Apply our changes to the fallback variables.
	*pWeapon->GetFallbackPaintKit() = g_SkinChangerCfg[nWeaponIndex].nFallbackPaintKit;
	*pWeapon->GetEntityQuality() = g_SkinChangerCfg[nWeaponIndex].iEntityQuality;
	*pWeapon->GetFallbackSeed() = g_SkinChangerCfg[nWeaponIndex].nFallbackSeed;
	*pWeapon->GetFallbackStatTrak() = g_SkinChangerCfg[nWeaponIndex].nFallbackStatTrak;
	*pWeapon->GetFallbackWear() = g_SkinChangerCfg[nWeaponIndex].flFallbackWear;

	// force so stattrack works
	if (g_SkinChangerCfg[nWeaponIndex].nFallbackStatTrak != -1)
	{
		*pWeapon->GetAccountID() = *pWeapon->GetFallbackOriginalOwnerXuidLow();
	}
	

	// If a name is defined, write it now.
	if (g_SkinChangerCfg[nWeaponIndex].szCustomName) {
		sprintf_s(pWeapon->GetCustomName(), 32, "%s", g_SkinChangerCfg[nWeaponIndex].szCustomName);
	}

	// Edit "m_iItemIDHigh" so fallback values will be used.
	*pWeapon->GetItemIDHigh() = -1;

	return true;
}

void Skinchanger::FSN(int stage)
{
	Init();

	if (stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START) {
		// Don't change anything if we're not alive.
		if (!G::Localplayer || G::Localplayer->GetLifeState() != LIFE_ALIVE)
			return;

		const auto highestEntityIndex = I::entitylist->GetHighestEntityIndex();
		for (int i = I::globalvars->nMaxClients + 1; i <= highestEntityIndex; ++i) {
			const auto entity = I::entitylist->GetClientEntity(i);
			if (!entity)
				continue;
				
			const auto weapon = entity;
			if (I::entitylist->GetClientEntityFromHandle(weapon->GetOwner()) != G::Localplayer)
				continue;

			ApplyCustomSkin(weapon);
		}


		static float lastTime = I::globalvars->flCurrentTime;
		if (fabsf(lastTime - I::globalvars->flCurrentTime) > 5)
		{
			lastTime = I::globalvars->flCurrentTime;
			if(G::cmd->iButtons & IN_USE)
				I::clientstate->iDeltaTick = -1;
			//static auto ForceUpdate = (void(__cdecl*)())FindPattern("engine.dll", "A1 ? ? ? ? B9 ? ? ? ? 56 FF 50 14 8B 34 85");
			//ForceUpdate();
			
		}

		return;
	}
}
