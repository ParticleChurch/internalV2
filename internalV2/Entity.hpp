#pragma once

// temporary for now
#define DEBUG_ENTITY false

// ENUM here for Virtual Functions (for easy changing)

// NOTE: i shouldn't be using all of these as references, im just assuming, will go through and fix later
// rule of thumb: if it doesn't work, remove the "&" :)

class Entity
{
public: // NETVARS
	// bool
	bool GetGunGameImmunity() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetGunGameImmunity");
		static DWORD offset = N::GetOffset("DT_CSPlayer", "m_bGunGameImmunity");
		return *(int*)((DWORD)this + offset);
	}
	bool HasHelmet() {
		if constexpr (DEBUG_ENTITY) L::Debug("HasHelmet");
		static DWORD offset = N::GetOffset("DT_CSPlayer", "m_bHasHelmet");
		return *(float*)((DWORD)this + offset);
	}
	bool* ClientAnimations() {
		if constexpr (DEBUG_ENTITY) L::Debug("ClientAnimations");
		static DWORD offset = N::GetOffset("DT_BaseAnimating", "m_bClientSideAnimation");
		return (bool*)(DWORD(this) + offset);
	}
	// int
	int& GetHealth()
	{
		if constexpr (DEBUG_ENTITY) L::Debug("GetHealth");
		static DWORD offset = N::GetOffset("DT_BasePlayer", "m_iHealth");
		return *(int*)((DWORD)this + offset);
	}
	int& GetTeam() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetTeam");
		static DWORD offset = N::GetOffset("DT_CSPlayer", "m_iTeamNum");
		return *(int*)((DWORD)this + offset);
	}
	int& GetMoveType() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetMoveType");
		static DWORD offset = N::GetOffset("DT_CSPlayer", "m_nRenderMode") + 1;
		return *(int*)((DWORD)this + offset);
	}
	int& GetFlags() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetFlags");
		static DWORD offset = N::GetOffset("DT_CSPlayer", "m_fFlags");
		return *(int*)((DWORD)this + offset);
	}
	int& GetTickBase() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetTickBase");
		static DWORD offset = N::GetOffset("DT_BasePlayer", "m_nTickBase");
		return *(int*)((DWORD)this + offset);
	}
	int& GetObserverMode(){
		if constexpr (DEBUG_ENTITY) L::Debug("GetObserverMode");
		static DWORD offset = N::GetOffset("DT_BasePlayer", "m_iObserverMode");
		return *(int*)((DWORD)this + offset);
	}
	int& GetViewModelID() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetViewModelID");
		static DWORD offset = N::GetOffset("DT_BaseCombatWeapon", "m_iViewModelIndex");
		return *(int*)((DWORD)this + offset);
	}
	int& GetShotsFired() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetShotsFired");
		static DWORD offset = N::GetOffset("DT_CSPlayer", "m_iShotsFired");
		return *(int*)((DWORD)this + offset);
	}
	int& ArmorVal() {
		if constexpr (DEBUG_ENTITY) L::Debug("ArmorVal");
		static DWORD offset = N::GetOffset("DT_CSPlayer", "m_ArmorValue");
		return *(int*)((DWORD)this + offset);
	}
	int& ScopeLevel() {
		if constexpr (DEBUG_ENTITY) L::Debug("ScopeLevel");
		static DWORD offset = N::GetOffset("DT_WeaponCSBaseGun", "m_zoomLevel");
		return *(int*)((DWORD)this + offset);
	}
	// char
	char& GetLifeState() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetLifeState");
		static DWORD offset = N::GetOffset("DT_BasePlayer", "m_lifeState");
		return *(char*)((DWORD)this + offset);
	}
	// float
	float& GetSimulationTime() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetSimulationTime");
		static DWORD offset = N::GetOffset("DT_BaseAnimating", "m_flSimulationTime");
		return *(float*)((DWORD)this + offset);
	}
	float& GetNextAttack() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetNextAttack");
		static DWORD offset = N::GetOffset("DT_BaseCombatCharacter", "m_flNextAttack");
		return *(float*)((DWORD)this + offset);
	}
	float& GetNextPrimaryAttack() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetNextPrimaryAttack");
		static DWORD offset = N::GetOffset("DT_BaseCombatWeapon", "m_flNextPrimaryAttack");
		return *(float*)((DWORD)this + offset);
	}
	float& GetFlashMaxAlpha() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetFlashMaxAlpha");
		static DWORD offset = N::GetOffset("DT_CSPlayer", "m_flFlashMaxAlpha");
		return *(float*)((DWORD)this + offset);
	}
	float& GetNextSecondaryAttack() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetNextSecondaryAttack");
		static DWORD offset = N::GetOffset("DT_BaseCombatWeapon", "m_flNextSecondaryAttack");
		return *(float*)((DWORD)this + offset);
	}
	float& GetGrenadeThrowTime() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetGrenadeThrowTime");
		static DWORD offset = N::GetOffset("DT_BaseCSGrenade", "m_fThrowTime");
		return *(float*)((DWORD)this + offset);
	}
	float& GetLastShotTime() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetLastShotTime");
		static DWORD offset = N::GetOffset("DT_WeaponCSBase", "m_fLastShotTime");
		return *(float*)((DWORD)this + offset);
	}
	float& GetFireReadyTime() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetLastShotTime");
		static DWORD offset = N::GetOffset("DT_WeaponCSBase", "m_flPostponeFireReadyTime");
		return *(float*)((DWORD)this + offset);
	}
	float& GetFlashMaxAlpha() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetLastShotTime");
		static DWORD offset = N::GetOffset("DT_CSPlayer", "m_flFlashMaxAlpha");
		return *(float*)((DWORD)this + offset);
	}
	float& GetLBY() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetLBY");
		static DWORD offset = N::GetOffset("DT_CSPlayer", "m_flLowerBodyYawTarget");
		return *(float*)((DWORD)this + offset);
	}
	float& GetDuckAmount() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetDuckAmount");
		static DWORD offset = N::GetOffset("DT_CSPlayer", "m_flDuckAmount");
		return *(float*)((DWORD)this + offset);
	}
	// float array
	std::array< float, 24 >& GetPoseParameters() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetPoseParameters");
		static DWORD offset = N::GetOffset("DT_BaseAnimating", "m_flPoseParameter");
		return *reinterpret_cast<std::array<float, 24>*>(reinterpret_cast<uintptr_t>(this) + offset);
	}
	// vector
	Vector& GetVecVelocity() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetVecVelocity");
		static DWORD offset = N::GetOffset("DT_BasePlayer", "m_vecVelocity[0]");
		return *(Vector*)((DWORD)this + offset);
	}
	Vector& GetVecOrigin() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetVecOrigin");
		static DWORD offset = N::GetOffset("DT_BasePlayer", "m_vecOrigin");
		return *(Vector*)((DWORD)this + offset);
	}
	Vector GetAimPunchAngle() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetAimPunchAngle");
		static DWORD offset = N::GetOffset("DT_BasePlayer", "m_aimPunchAngle");
		return *(Vector*)((DWORD)this + offset);
	}
	Vector& GetViewPunchAngle() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetViewPunchAngle");
		static DWORD offset = N::GetOffset("DT_BasePlayer", "m_viewPunchAngle");
		return *(Vector*)((DWORD)this + offset);
	}
	Vector& GetMins() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetMins");
		static DWORD offset = N::GetOffset("DT_BaseEntity", "m_vecMins");
		return *(Vector*)((DWORD)this + offset);
	}
	Vector& GetMaxs() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetMaxs");
		static DWORD offset = N::GetOffset("DT_BaseEntity", "m_vecMaxs");
		return *(Vector*)((DWORD)this + offset);
	}
	Vector& GetEyeHeight() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetEyeHeight");
		static DWORD offset = N::GetOffset("DT_CSPlayer", "m_vecViewOffset[0]");
		return *(Vector*)((DWORD)this + offset);
	}
	// HANDLE
	HANDLE& m_hObserverTarget() {
		if constexpr (DEBUG_ENTITY) L::Debug("m_hObserverTarget");
		static DWORD offset = N::GetOffset("DT_CSPlayer", "m_hObserverTarget");
		return *(HANDLE*)((DWORD)this + offset);
	}
	HANDLE& GetOwner()
	{
		static DWORD offset = N::GetOffset("DT_BaseEntity", "m_hOwnerEntity");
		return *(HANDLE*)((DWORD)this + offset);
	}
public: // VIRTUALS 
	
public: // OTHERS

};