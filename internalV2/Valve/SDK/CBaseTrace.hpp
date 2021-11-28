#pragma once
class CBaseTrace
{
public:
	CBaseTrace() { }

	Vector			vecStart;		// start position
	Vector			vecEnd;			// final position
	cplane_t		plane;			// surface normal at impact
	float			flFraction;		// time completed, 1.0 = didn't hit anything
	int				iContents;		// contents on other side of surface hit
	std::uint16_t	fDispFlags;		// displacement flags for marking surfaces with data
	bool			bAllSolid;		// if true, plane is not valid
	bool			bStartSolid;	// if true, the initial point was in a solid area
};