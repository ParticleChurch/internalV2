#pragma once
class CGameTrace : public CBaseTrace
{
public:
	CGameTrace() : pHitEntity(nullptr) { }

	float				flFractionLeftSolid;	// time we left a solid, only valid if we started in solid
	csurface_t			surface;				// surface hit (impact surface)
	int					iHitGroup;				// 0 == generic, non-zero is specific body part
	short				sPhysicsBone;			// physics bone hit by trace in studio
	std::uint16_t		uWorldSurfaceIndex;		// index of the msurface2_t, if applicable
	CBaseEntity* pHitEntity;				// entity hit by trace
	int					iHitbox;				// box hit by trace in studio

	inline bool DidHit() const
	{
		return (flFraction < 1.0f || bAllSolid || bStartSolid);
	}

	inline bool IsVisible() const
	{
		return (flFraction > 0.97f);
	}

private:
	CGameTrace(const CGameTrace& other)
	{
		this->vecStart = other.vecStart;
		this->vecEnd = other.vecEnd;
		this->plane = other.plane;
		this->flFraction = other.flFraction;
		this->iContents = other.iContents;
		this->fDispFlags = other.fDispFlags;
		this->bAllSolid = other.bAllSolid;
		this->bStartSolid = other.bStartSolid;
		this->flFractionLeftSolid = other.flFractionLeftSolid;
		this->surface = other.surface;
		this->iHitGroup = other.iHitGroup;
		this->sPhysicsBone = other.sPhysicsBone;
		this->uWorldSurfaceIndex = other.uWorldSurfaceIndex;
		this->pHitEntity = other.pHitEntity;
		this->iHitbox = other.iHitbox;
	}
};
using Trace_t = CGameTrace;