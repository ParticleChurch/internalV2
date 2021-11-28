#pragma once
#define IN_ATTACK		(1 << 0)
#define IN_JUMP			(1 << 1)
#define IN_DUCK			(1 << 2)
#define IN_FORWARD		(1 << 3)
#define IN_BACK			(1 << 4)
#define IN_USE			(1 << 5)
#define IN_CANCEL		(1 << 6)
#define IN_LEFT			(1 << 7)
#define IN_RIGHT		(1 << 8)
#define IN_MOVELEFT		(1 << 9)
#define IN_MOVERIGHT	(1 << 10)
#define IN_ATTACK2		(1 << 11)
#define IN_RUN			(1 << 12)
#define IN_RELOAD		(1 << 13)
#define IN_ALT1			(1 << 14)
#define IN_ALT2			(1 << 15)
#define IN_SCORE		(1 << 16)   // Used by client.dll for when scoreboard is held down
#define IN_SPEED		(1 << 17)	// Player is holding the speed key
#define IN_WALK			(1 << 18)	// Player holding walk key
#define IN_ZOOM			(1 << 19)	// Zoom key for HUD zoom
#define IN_WEAPON1		(1 << 20)	// weapon defines these bits
#define IN_WEAPON2		(1 << 21)	// weapon defines these bits
#define IN_BULLRUSH		(1 << 22)
#define IN_GRENADE1		(1 << 23)	// grenade 1
#define IN_GRENADE2		(1 << 24)	// grenade 2

struct CUserCmd
{
	void* vmt;
	int				iCommandNumber;		// 0x04
	int				iTickCount;			// 0x08
	QAngle			angViewPoint;		// 0x0C
	Vector			vecAimDirection;	// 0x18
	float			flForwardMove;		// 0x24
	float			flSideMove;			// 0x28
	float			flUpMove;			// 0x2C
	int				iButtons;			// 0x30
	std::uint8_t	uImpulse;			// 0x34
	int				iWeaponSelect;		// 0x38
	int				iWeaponSubType;		// 0x3C
	int				iRandomSeed;		// 0x40
	short			sMouseDeltaX;		// 0x44
	short			sMouseDeltaY;		// 0x46
	bool			bHasBeenPredicted;	// 0x48
	Vector			vecHeadAngles;		// 0x4C
	Vector			vecHeadOffset;		// 0x58
};