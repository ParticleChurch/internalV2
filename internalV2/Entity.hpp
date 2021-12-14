#pragma once
// temporary for now
#define DEBUG_ENTITY false

// Enum for team
enum Team : int
{
	TEAM_UNASSIGNED = 0,
	TEAM_SPECTATOR,
	TEAM_TT,
	TEAM_CT
};

// Enum here for Virtual Functions (for easy changing)
enum Entity_VFunc_A : int
{
	GetAbsOrigin = 10,
	GetCollideable = 3
	
};
enum Entity_VFunc_B : int
{
	IsDormant = 9,
	SetupBones = 13
};

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
	bool HasHeavyArmor() {
		if constexpr (DEBUG_ENTITY) L::Debug("HasHeavyArmor");
		static DWORD offset = N::GetOffset("DT_CSPlayer", "m_bHasHeavyArmor");
		return *(float*)((DWORD)this + offset);
	}
	bool* ClientAnimations() {
		if constexpr (DEBUG_ENTITY) L::Debug("ClientAnimations");
		static DWORD offset = N::GetOffset("DT_BaseAnimating", "m_bClientSideAnimation");
		return (bool*)(DWORD(this) + offset);
	}
	bool IsBroken() { //  for CBreakableSurface
		if constexpr (DEBUG_ENTITY) L::Debug("IsBroken");
		static DWORD offset = N::GetOffset("CBreakableSurface", "m_bIsBroken");
		return *(bool*)(DWORD(this) + offset);
	}
	// int
	int GetHealth() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetHealth");
		static DWORD offset = N::GetOffset("DT_BasePlayer", "m_iHealth");
		return *(int*)((DWORD)this + offset);
	}
	int GetTeam() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetTeam");
		static DWORD offset = N::GetOffset("DT_CSPlayer", "m_iTeamNum");
		return *(int*)((DWORD)this + offset);
	}
	int GetMoveType() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetMoveType");
		static DWORD offset = N::GetOffset("DT_CSPlayer", "m_nRenderMode") + 1;
		return *(int*)((DWORD)this + offset);
	}
	int GetFlags() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetFlags");
		static DWORD offset = N::GetOffset("DT_CSPlayer", "m_fFlags");
		return *(int*)((DWORD)this + offset);
	}
	int GetTickBase() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetTickBase");
		static DWORD offset = N::GetOffset("DT_BasePlayer", "m_nTickBase");
		return *(int*)((DWORD)this + offset);
	}
	int GetObserverMode() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetObserverMode");
		static DWORD offset = N::GetOffset("DT_BasePlayer", "m_iObserverMode");
		return *(int*)((DWORD)this + offset);
	}
	int GetViewModelID() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetViewModelID");
		static DWORD offset = N::GetOffset("DT_BaseCombatWeapon", "m_iViewModelIndex");
		return *(int*)((DWORD)this + offset);
	}
	int GetShotsFired() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetShotsFired");
		static DWORD offset = N::GetOffset("DT_CSPlayer", "m_iShotsFired");
		return *(int*)((DWORD)this + offset);
	}
	int GetArmor() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetArmor");
		static DWORD offset = N::GetOffset("DT_CSPlayer", "m_ArmorValue");
		return *(int*)((DWORD)this + offset);
	}
	int ScopeLevel() {
		if constexpr (DEBUG_ENTITY) L::Debug("ScopeLevel");
		static DWORD offset = N::GetOffset("DT_WeaponCSBaseGun", "m_zoomLevel");
		return *(int*)((DWORD)this + offset);
	}
	int GetCollisionGroup() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetCollisionGroup");
		static DWORD offset = N::GetOffset("CBaseEntity", "m_CollisionGroup");
		return *(int*)((DWORD)this + offset);
	}
	//short
	short GetItemDefinitionIndex() {
		if (DEBUG_ENTITY) L::Debug("GetItemDefinitionIndex");
		static DWORD offset = N::GetOffset("DT_BaseAttributableItem", "m_iItemDefinitionIndex");
		return *(short*)((DWORD)this + offset);
	}
	// char
	char GetLifeState() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetLifeState");
		static DWORD offset = N::GetOffset("DT_BasePlayer", "m_lifeState");
		return *(char*)((DWORD)this + offset);
	}
	// float
	float GetSimulationTime() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetSimulationTime");
		static DWORD offset = N::GetOffset("DT_BaseAnimating", "m_flSimulationTime");
		return *(float*)((DWORD)this + offset);
	}
	float GetNextAttack() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetNextAttack");
		static DWORD offset = N::GetOffset("DT_BaseCombatCharacter", "m_flNextAttack");
		return *(float*)((DWORD)this + offset);
	}
	float GetNextPrimaryAttack() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetNextPrimaryAttack");
		static DWORD offset = N::GetOffset("DT_BaseCombatWeapon", "m_flNextPrimaryAttack");
		return *(float*)((DWORD)this + offset);
	}
	float GetFlashMaxAlpha() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetFlashMaxAlpha");
		static DWORD offset = N::GetOffset("DT_CSPlayer", "m_flFlashMaxAlpha");
		return *(float*)((DWORD)this + offset);
	}
	float GetNextSecondaryAttack() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetNextSecondaryAttack");
		static DWORD offset = N::GetOffset("DT_BaseCombatWeapon", "m_flNextSecondaryAttack");
		return *(float*)((DWORD)this + offset);
	}
	float GetGrenadeThrowTime() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetGrenadeThrowTime");
		static DWORD offset = N::GetOffset("DT_BaseCSGrenade", "m_fThrowTime");
		return *(float*)((DWORD)this + offset);
	}
	float GetLastShotTime() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetLastShotTime");
		static DWORD offset = N::GetOffset("DT_WeaponCSBase", "m_fLastShotTime");
		return *(float*)((DWORD)this + offset);
	}
	float GetFireReadyTime() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetLastShotTime");
		static DWORD offset = N::GetOffset("DT_WeaponCSBase", "m_flPostponeFireReadyTime");
		return *(float*)((DWORD)this + offset);
	}
	float GetLBY() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetLBY");
		static DWORD offset = N::GetOffset("DT_CSPlayer", "m_flLowerBodyYawTarget");
		return *(float*)((DWORD)this + offset);
	}
	float GetDuckAmount() {
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
	Vector GetVecVelocity() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetVecVelocity");
		static DWORD offset = N::GetOffset("DT_BasePlayer", "m_vecVelocity[0]");
		return *(Vector*)((DWORD)this + offset);
	}
	Vector GetVecOrigin() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetVecOrigin");
		static DWORD offset = N::GetOffset("DT_BasePlayer", "m_vecOrigin");
		return *(Vector*)((DWORD)this + offset);
	}
	Vector GetAimPunchAngle() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetAimPunchAngle");
		static DWORD offset = N::GetOffset("DT_BasePlayer", "m_aimPunchAngle");
		return *(Vector*)((DWORD)this + offset);
	}
	Vector GetViewPunchAngle() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetViewPunchAngle");
		static DWORD offset = N::GetOffset("DT_BasePlayer", "m_viewPunchAngle");
		return *(Vector*)((DWORD)this + offset);
	}
	Vector GetMins() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetMins");
		static DWORD offset = N::GetOffset("DT_BaseEntity", "m_vecMins");
		return *(Vector*)((DWORD)this + offset);
	}
	Vector GetMaxs() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetMaxs");
		static DWORD offset = N::GetOffset("DT_BaseEntity", "m_vecMaxs");
		return *(Vector*)((DWORD)this + offset);
	}
	Vector GetEyeHeight() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetEyeHeight");
		static DWORD offset = N::GetOffset("DT_CSPlayer", "m_vecViewOffset[0]");
		return *(Vector*)((DWORD)this + offset);
	}
	// HANDLE
	HANDLE m_hObserverTarget() {
		if constexpr (DEBUG_ENTITY) L::Debug("m_hObserverTarget");
		static DWORD offset = N::GetOffset("DT_CSPlayer", "m_hObserverTarget");
		return *(HANDLE*)((DWORD)this + offset);
	}
	HANDLE GetOwner() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetOwner");
		static DWORD offset = N::GetOffset("DT_BaseEntity", "m_hOwnerEntity");
		return *(HANDLE*)((DWORD)this + offset);
	}
public: // PATTERNS
	void SetAbsOrigin(const Vector& vecOrigin) {
		if constexpr (DEBUG_ENTITY) L::Debug("SetAbsOrigin");
		using SetAbsOriginFn = void(__thiscall*)(void*, const Vector&);
		static auto oSetAbsOrigin = reinterpret_cast<SetAbsOriginFn>(FindPattern("client.dll", "55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8"));
		oSetAbsOrigin(this, vecOrigin);
	}
	int& GetTakeDamage() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetTakeDamage");
		static const std::uintptr_t uTakeDamageOffset = *reinterpret_cast<std::uintptr_t*>(FindPattern("client.dll", "80 BE ? ? ? ? ? 75 46 8B 86") + 0x2);
		return *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + uTakeDamageOffset);
	}
public: // VIRTUALS
	Vector GetAbsOrigin() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetAbsOrigin");
		typedef bool(__thiscall* oIsDormant)(void*);
		return VMT::GetVirtualMethod<oIsDormant>(this, Entity_VFunc_A::GetAbsOrigin)(this);
	}
	bool SetupBones(matrix3x4_t* out, int maxBones, int boneMask, float currentTime, bool fixbonematrix = false)
	{
		typedef bool(__thiscall* oSetupBones)(void*, matrix3x4_t*, int, int, float);
		if (fixbonematrix) {
			int* render = reinterpret_cast<int*>(this + 0x274);
			int backup = *render;
			Vector absOrigin = GetAbsOrigin();
			*render = 0;
			SetAbsOrigin(GetVecOrigin());
			auto result = VMT::GetVirtualMethod<oSetupBones>(this + sizeof(uintptr_t), Entity_VFunc_B::SetupBones)(this + sizeof(uintptr_t), out, maxBones, boneMask, currentTime);
			SetAbsOrigin(absOrigin);
			*render = backup;
			return result;
		}
		return VMT::GetVirtualMethod<oSetupBones>(this + 4, Entity_VFunc_B::SetupBones)(this + 4, out, maxBones, boneMask, currentTime);
	}
	bool IsDormant() {
		if constexpr (DEBUG_ENTITY) L::Debug("IsDormant");
		typedef bool(__thiscall* oIsDormant)(void*);
		return VMT::GetVirtualMethod<oIsDormant>(this + sizeof(uintptr_t) * 2, Entity_VFunc_B::IsDormant)(this + sizeof(uintptr_t) * 2);
	}
	ICollideable* GetCollideable() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetCollideable");
		typedef ICollideable*(__thiscall* oGetCollideable)(void*);
		return VMT::GetVirtualMethod<oGetCollideable>(this, Entity_VFunc_A::GetCollideable)(this);
	}
	int IsMaxHealth() {
		if constexpr (DEBUG_ENTITY) L::Debug("IsMaxHealth");
		// @ida: client.dll @ [FF 90 ? ? ? ? 85 C0 0F 8F ? ? ? ? 80 + 0x2] / sizeof(std::uintptr_t)
		return CallVFunc<int>(this, 123);
	}
	CBaseClient* GetClientClass() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetClientClass");
		typedef CBaseClient* (__thiscall* oGetClientClass)(void*);
		return GetVFunc<oGetClientClass>(this + 8, 2)(this + 8);
	}
	const char* GetClassname() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetClassname");
		// @ida: client.dll @ [8B 01 FF 90 ? ? ? ? 90 + 0x4] / sizeof(std::uintptr_t)
		return CallVFunc<const char*>(this, 143);
	}
	unsigned int PhysicsSolidMaskForEntity() {
		if constexpr (DEBUG_ENTITY) L::Debug("PhysicsSolidMaskForEntity");
		// @xref: "func_breakable", "func_breakable_surf"
		return CallVFunc<unsigned int>(this, 152);
	}
	int GetIndex() {
		if (DEBUG_ENTITY) L::Debug("GetIndex");
		typedef int(__thiscall* oGetIndex)(void*);
		return GetVFunc<oGetIndex>(this + 8, 10)(this + 8);
	}
	const matrix3x4_t& GetTransform()
	{
		if (DEBUG_ENTITY) L::Debug("GetTransform");

		typedef const matrix3x4_t& (__thiscall* oGetTransform)(void*);
		return GetVFunc<oGetTransform>(this + sizeof(uintptr_t), 3)(this + sizeof(uintptr_t));
	}
	Model_t* GetModel()
	{
		if (DEBUG_ENTITY) L::Debug("Model_t");
		typedef Model_t* (__thiscall* oGetModel)(void*);
		return GetVFunc<oGetModel>(this + sizeof(uintptr_t), 8)(this + sizeof(uintptr_t));
	}
	bool IsPlayer() {
		if (DEBUG_ENTITY) L::Debug("IsPlayer");
		// @xref: "effects/nightvision"
		return CallVFunc<bool>(this, 158);
	}
	float GetSpread() {
		if (DEBUG_ENTITY) L::Debug("GetSpread");
		return CallVFunc<float>(this, 453);
	}
	float GetInaccuracy() {
		if (DEBUG_ENTITY) L::Debug("GetInaccuracy");
		return CallVFunc<float>(this, 483);
	}
public: // OTHERS
	Entity* GetActiveWeapon() {
		if (DEBUG_ENTITY) L::Debug("GetActiveWeapon");
		static DWORD offset = N::GetOffset("DT_BasePlayer", "m_hActiveWeapon");
		return I::entitylist->GetClientEntityFromHandle(*(HANDLE*)((DWORD)this + offset));
	}
	CCSWeaponData* GetWeaponData() {
		if (DEBUG_ENTITY) L::Debug("GetWeaponData");
		short nDefinitionIndex = this->GetItemDefinitionIndex();
		return I::weaponsystem->GetWeaponData(nDefinitionIndex);
	}
	inline bool IsAlive()
	{
		// @note: https://github.com/rollraw/qo0-base/issues/135
		return (this->GetLifeState() == LIFE_ALIVE);
	}
	Vector GetEyePosition() {
		if (DEBUG_ENTITY) L::Debug("GetEyePosition");
		static DWORD offset = N::GetOffset("DT_CSPlayer", "m_vecViewOffset[0]");
		return (GetVecOrigin() + *(Vector*)((DWORD)this + offset));
	}
	inline bool IsArmored(const int iHitGroup)
	{
		// @ida isarmored: server.dll @ 55 8B EC 32 D2

		bool bIsArmored = false;

		if (this->GetArmor() > 0)
		{
			switch (iHitGroup)
			{
			case HITGROUP_GENERIC:
			case HITGROUP_CHEST:
			case HITGROUP_STOMACH:
			case HITGROUP_LEFTARM:
			case HITGROUP_RIGHTARM:
			case HITGROUP_NECK:
				bIsArmored = true;
				break;
			case HITGROUP_HEAD:
				if (this->HasHelmet())
					bIsArmored = true;
				[[fallthrough]];
			case HITGROUP_LEFTLEG:
			case HITGROUP_RIGHTLEG:
				if (this->HasHeavyArmor())
					bIsArmored = true;
				break;
			default:
				break;
			}
		}

		return bIsArmored;
	}
	bool IsBreakable() {
		return false; // TEMPORARY
		if constexpr (DEBUG_ENTITY) L::Debug("IsBreakable");

		// @ida isbreakableentity: client.dll @ 55 8B EC 51 56 8B F1 85 F6 74 68
		/*
		const int iHealth = this->GetHealth();

		// first check to see if it's already broken
		if (iHealth < 0 && this->IsMaxHealth() > 0)
			return true;

		if (this->GetTakeDamage() != DAMAGE_YES)
		{
			const EClassIndex nClassIndex = this->GetClientClass()->nClassID;

			// force pass cfuncbrush
			if (nClassIndex != EClassIndex::CFuncBrush)
				return false;
		}

		if (const int nCollisionGroup = this->GetCollisionGroup(); nCollisionGroup != COLLISION_GROUP_PUSHAWAY && nCollisionGroup != COLLISION_GROUP_BREAKABLE_GLASS && nCollisionGroup != COLLISION_GROUP_NONE)
			return false;

		if (iHealth > 200)
			return false;

		if (IMultiplayerPhysics* pPhysicsInterface = (IMultiplayerPhysics*)(this); pPhysicsInterface != nullptr)
		{
			if (pPhysicsInterface->GetMultiplayerPhysicsMode() != PHYSICS_MULTIPLAYER_SOLID) // probably will cause issues idk
				return false;
		}
		else
		{
			if (const char* szClassName = this->GetClassname(); !strcmp(szClassName, "func_breakable") || !strcmp(szClassName, "func_breakable_surf"))
			{
				if (!strcmp(szClassName, "func_breakable_surf"))
				{
					Entity* pSurface = static_cast<Entity*>(this);

					// don't try to break it if it has already been broken
					if (pSurface->IsBroken())
						return false;
				}
			}
			else if (this->PhysicsSolidMaskForEntity() & CONTENTS_PLAYERCLIP)
			{
				// hostages and players use CONTENTS_PLAYERCLIP, so we can use it to ignore them
				return false;
			}
		}

		if (IBreakableWithPropData* pBreakableInterface = (IBreakableWithPropData*)(this); pBreakableInterface != nullptr)
		{
			// bullets don't damage it - ignore
			if (pBreakableInterface->GetDmgModBullet() <= 0.0f)
				return false;
		}

		return true;
		*/

		// My version cuz thats^ too confusing

		//m_takeDamage isn't properly set when using the signature.
		//Back it up, set it to true, then restore.
		int takeDamageBackup = this->GetTakeDamage();

		CBaseClient* pClass = this->GetClientClass();

		//				 '       ''     '      '   '
		//			    01234567890123456     012345678
		//Check against CBreakableSurface and CBaseDoor

		//Windows etc. are CBrekableSurface
		//Large garage door in Office is CBaseDoor and it get's reported as a breakable when it is not one
		//This is seperate from "CPropDoorRotating", which is a normal door.
		//Normally you would also check for "CFuncBrush" but it was acting oddly so I removed it. It's below if interested.
		//((clientClass->m_pNetworkName[1]) != 'F' || (clientClass->m_pNetworkName[4]) != 'c' || (clientClass->m_pNetworkName[5]) != 'B' || (clientClass->m_pNetworkName[9]) != 'h')

		if ((pClass->szNetworkName[1] == 'B' && pClass->szNetworkName[9] == 'e' && pClass->szNetworkName[10] == 'S' && pClass->szNetworkName[16] == 'e')
			|| (pClass->szNetworkName[1] != 'B' || pClass->szNetworkName[5] != 'D'))
			((Entity*)this)->GetTakeDamage() = DAMAGE_YES;

		using IsBreakableEntityFn = bool(__thiscall*)(Entity*);
		static auto oIsBreakableEntity = reinterpret_cast<IsBreakableEntityFn>(FindPattern("client.dll", "55 8B EC 51 56 8B F1 85 F6 74 68"));

		bool breakable = oIsBreakableEntity(this);
		((Entity*)this)->GetTakeDamage() = takeDamageBackup;

		return breakable;
	}
	float GetServerTime() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetServerTime");
		return GetTickBase() * I::globalvars->flIntervalPerTick;
	}
	bool CanShoot() {
		if constexpr (DEBUG_ENTITY) L::Debug("CanShoot");
		Entity* weap = this->GetActiveWeapon();
		if (!weap)
			return false;
		float ServerTime = this->GetServerTime();
		return this->GetNextAttack() <= ServerTime && weap->GetNextPrimaryAttack() <= ServerTime;
	}
	matrix3x4_t* GetCoordinateFrame() {
		if constexpr (DEBUG_ENTITY) L::Debug("GetCoordinateFrame");
		// Coordinate frame is located at m_CollisionGroup - 0x30
		static DWORD offset = N::GetOffset("DT_BaseEntity", "m_CollisionGroup") - 0x30;
		return (matrix3x4_t*)((DWORD)this + offset);
	}
	Vector GetLeft(Vector C, float radius, Entity* ent) {
		if (DEBUG_ENTITY) L::Debug("GetLeft");
		Vector P = ent->GetEyePosition();	//player
		float gamma = (M_PI / 2.f) + atan2f(C.y - P.y, C.x - P.x);
		float distance = sqrtf(pow((C.x - P.x), 2) + pow((C.y - P.y), 2));
		float beta = asin(radius / distance);

		return Vector(C.x + radius * cosf(gamma + beta), C.y + radius * sinf(gamma + beta), C.z);
	}
	Vector GetRight(Vector C, float radius, Entity* ent) {
		if (DEBUG_ENTITY) L::Debug("GetRight");
		Vector P = ent->GetEyePosition();	//player
		float gamma = (M_PI / 2.f) + atan2f(C.y - P.y, C.x - P.x);
		float distance = sqrtf(pow((C.x - P.x), 2) + pow((C.y - P.y), 2));
		float beta = asin(radius / distance);

		return Vector(C.x - radius * cosf(gamma - beta), C.y - radius * sinf(gamma - beta), C.z);
	}
	Vector GetTopLeft(Vector C, float radius, Entity* ent) {
		if (DEBUG_ENTITY) L::Debug("GetTopLeft");
		Vector P = ent->GetEyePosition();	//player
		float gamma = (M_PI / 2.f) + atan2f(C.y - P.y, C.x - P.x);
		float distance = sqrtf(pow((C.x - P.x), 2) + pow((C.y - P.y), 2));
		float beta = asin(radius / distance);

		return Vector(C.x + .7071 * radius * cosf(gamma + beta), C.y + .7071 * radius * sinf(gamma + beta), C.z + radius * .7071);
	}
	Vector GetTopRight(Vector C, float radius, Entity* ent) {
		if (DEBUG_ENTITY) L::Debug("GetTopRight");
		Vector P = ent->GetEyePosition();	//player
		float gamma = (M_PI / 2.f) + atan2f(C.y - P.y, C.x - P.x);
		float distance = sqrtf(pow((C.x - P.x), 2) + pow((C.y - P.y), 2));
		float beta = asin(radius / distance);

		return Vector(C.x - .7071 * radius * cosf(gamma + beta), C.y - .7071 * radius * sinf(gamma + beta), C.z + radius * .7071);
	}
	Vector GetBottomLeft(Vector C, float radius, Entity* ent) {
		if (DEBUG_ENTITY) L::Debug("GetBottomLeft");
		Vector P = ent->GetEyePosition();	//player
		float gamma = (M_PI / 2.f) + atan2f(C.y - P.y, C.x - P.x);
		float distance = sqrtf(pow((C.x - P.x), 2) + pow((C.y - P.y), 2));
		float beta = asin(radius / distance);

		return Vector(C.x + .7071 * radius * cosf(gamma + beta), C.y + .7071 * radius * sinf(gamma + beta), C.z - radius * .7071);
	}
	Vector GetBottomRight(Vector C, float radius, Entity* ent) {
		if (DEBUG_ENTITY) L::Debug("GetBottomRight");
		Vector P = ent->GetEyePosition();	//player
		float gamma = (M_PI / 2.f) + atan2f(C.y - P.y, C.x - P.x);
		float distance = sqrtf(pow((C.x - P.x), 2) + pow((C.y - P.y), 2));
		float beta = asin(radius / distance);

		return Vector(C.x - .7071 * radius * cosf(gamma + beta), C.y - .7071 * radius * sinf(gamma + beta), C.z - radius * .7071);
	}
	int GetAmmo() {
		if (DEBUG_ENTITY) L::Debug("GetAmmo");
		static DWORD offset = N::GetOffset("DT_BaseCombatWeapon", "m_iClip1");
		Entity* weap = this->GetActiveWeapon();
		if (!weap)
			return 0;
		return *(int*)(weap + offset);
	}
	CCSGOPlayerAnimState* GetAnimationState() {
		if (DEBUG_ENTITY) L::Debug("GetSpread");
		static DWORD offset = N::GetOffset("DT_CSPlayer", "m_bIsScoped") - 0x14;
		return (CCSGOPlayerAnimState*)((DWORD)this + offset);
	}
	float GetMaxDesyncDelta() {
		CCSGOPlayerAnimState* pAnimState = this->GetAnimationState();
		if (!pAnimState) return 0.f;

		float flDuckAmount = pAnimState->flDuckAmount;
		float flRunningSpeed = std::clamp(pAnimState->flRunningSpeed, 0.0f, 1.0f);
		float flDuckingSpeed = std::clamp(pAnimState->flDuckingSpeed, 0.0f, 1.0f);
		float flYawModifier = (((pAnimState->flWalkToRunTransition * -0.3f) - 0.2f) * flRunningSpeed) + 1.0f;

		if (flDuckAmount > 0.0f)
			flYawModifier += ((flDuckAmount * flDuckingSpeed) * (0.5f - flYawModifier));

		float flMaxYawModifier = flYawModifier * pAnimState->flMaxBodyYaw;
		return flMaxYawModifier;
	}
};

