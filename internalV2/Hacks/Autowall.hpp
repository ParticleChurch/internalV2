struct FireBulletData_t
{
	Vector			vecPosition = { };
	Vector			vecDirection = { };
	Trace_t			enterTrace = { };
	float			flCurrentDamage = 0.0f;
	int				iPenetrateCount = 0;
};

// @credits: outlassn
class CAutoWall
{
public:
	/* returns damage at point and simulated bullet data (if given) */
	static float GetDamage(Vector start, Entity* pLocal, const Vector& vecPoint, FireBulletData_t* pDataOut = nullptr);
	/* returns damage at point and simulated bullet data (if given) */
	static float GetDamage(Entity* pLocal, const Vector& vecPoint, FireBulletData_t* pDataOut = nullptr);
	/* calculates damage factor */
	static void ScaleDamage(const int iHitGroup, Entity* pEntity, const float flWeaponArmorRatio, const float flWeaponHeadShotMultiplier, float& flDamage);
	/* simulates fire bullet to penetrate up to 4 walls, return true when hitting player */
	static bool SimulateFireBullet(Entity* pLocal, Entity* pWeapon, FireBulletData_t& data);

public:
	// Main
	static void ClipTraceToPlayers(const Vector& vecAbsStart, const Vector& vecAbsEnd, const unsigned int fMask, ITraceFilter* pFilter, Trace_t* pTrace, const float flMinRange = 0.0f);
	static bool TraceToExit(Trace_t& enterTrace, Trace_t& exitTrace, const Vector& vecPosition, const Vector& vecDirection, const Entity* pClipPlayer);
	static bool HandleBulletPenetration(Entity* pLocal, const CCSWeaponData* pWeaponData, const surfacedata_t* pEnterSurfaceData, FireBulletData_t& data);
};

extern CAutoWall* autowall;
