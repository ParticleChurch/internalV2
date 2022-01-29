/* max animation layers */
#define MAXOVERLAYS 15

enum ESequenceActivity : int
{
	ACT_CSGO_NULL = 957,
	ACT_CSGO_DEFUSE,
	ACT_CSGO_DEFUSE_WITH_KIT,
	ACT_CSGO_FLASHBANG_REACTION,
	ACT_CSGO_FIRE_PRIMARY,
	ACT_CSGO_FIRE_PRIMARY_OPT_1,
	ACT_CSGO_FIRE_PRIMARY_OPT_2,
	ACT_CSGO_FIRE_SECONDARY,
	ACT_CSGO_FIRE_SECONDARY_OPT_1,
	ACT_CSGO_FIRE_SECONDARY_OPT_2,
	ACT_CSGO_RELOAD,
	ACT_CSGO_RELOAD_START,
	ACT_CSGO_RELOAD_LOOP,
	ACT_CSGO_RELOAD_END,
	ACT_CSGO_OPERATE,
	ACT_CSGO_DEPLOY,
	ACT_CSGO_CATCH,
	ACT_CSGO_SILENCER_DETACH,
	ACT_CSGO_SILENCER_ATTACH,
	ACT_CSGO_TWITCH,
	ACT_CSGO_TWITCH_BUYZONE,
	ACT_CSGO_PLANT_BOMB,
	ACT_CSGO_IDLE_TURN_BALANCEADJUST,
	ACT_CSGO_IDLE_ADJUST_STOPPEDMOVING,
	ACT_CSGO_ALIVE_LOOP,
	ACT_CSGO_FLINCH,
	ACT_CSGO_FLINCH_HEAD,
	ACT_CSGO_FLINCH_MOLOTOV,
	ACT_CSGO_JUMP,
	ACT_CSGO_FALL,
	ACT_CSGO_CLIMB_LADDER,
	ACT_CSGO_LAND_LIGHT,
	ACT_CSGO_LAND_HEAVY,
	ACT_CSGO_EXIT_LADDER_TOP,
	ACT_CSGO_EXIT_LADDER_BOTTOM
};

class matrix3x4a_t;
class CBaseAnimating;
class CBoneAccessor
{
public:
	const CBaseAnimating* pAnimating;		//0x00
	matrix3x4a_t* matBones;		//0x04
	int						nReadableBones;	//0x08
	int						nWritableBones;	//0x0C
}; // Size: 0x10

class CAnimationLayer
{
public:
	float			flAnimationTime;		//0x00
	float			flFadeOut;				//0x04
	void* pStudioHdr;				//0x08
	int				nDispatchedSrc;			//0x0C
	int				nDispatchedDst;			//0x10
	int				iOrder;					//0x14
	std::uintptr_t  nSequence;				//0x18
	float			flPrevCycle;			//0x1C
	float			flWeight;				//0x20
	float			flWeightDeltaRate;		//0x24
	float			flPlaybackRate;			//0x28
	float			flCycle;				//0x2C
	void* pOwner;					//0x30
	int				nInvalidatePhysicsBits;	//0x34
}; // Size: 0x38

class CBaseEntity;
class CBaseCombatWeapon;
class CCSGOPlayerAnimState
{
public:
	void Create(CBaseEntity* pEntity)
	{
		using CreateAnimationStateFn = void(__thiscall*)(void*, CBaseEntity*);
		static auto oCreateAnimationState = reinterpret_cast<CreateAnimationStateFn>(FindPattern("client.dll", "55 8B EC 56 8B F1 B9 ? ? ? ? C7 46")); // @xref: "ggprogressive_player_levelup"

		if (oCreateAnimationState == nullptr)
			return;

		oCreateAnimationState(this, pEntity);
	}

	void Update(QAngle angView)
	{
		using UpdateAnimationStateFn = void(__vectorcall*)(void*, void*, float, float, float, void*);
		static auto oUpdateAnimationState = reinterpret_cast<UpdateAnimationStateFn>(FindPattern("client.dll", "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24")); // @xref: "%s_aim"

		if (oUpdateAnimationState == nullptr)
			return;

		oUpdateAnimationState(this, nullptr, 0.0f, angView.y, angView.x, nullptr);
	}

	void Reset()
	{
		using ResetAnimationStateFn = void(__thiscall*)(void*);
		static auto oResetAnimationState = reinterpret_cast<ResetAnimationStateFn>(FindPattern("client.dll", "56 6A 01 68 ? ? ? ? 8B F1")); // @xref: "player_spawn"

		if (oResetAnimationState == nullptr)
			return;

		oResetAnimationState(this);
	}

	std::byte	pad0[0x60]; // 0x00
	CBaseEntity* pEntity; // 0x60
	CBaseCombatWeapon* pActiveWeapon; // 0x64
	CBaseCombatWeapon* pLastActiveWeapon; // 0x68
	float		flLastUpdateTime; // 0x6C
	int			iLastUpdateFrame; // 0x70
	float		flLastUpdateIncrement; // 0x74
	float		flEyeYaw; // 0x78
	float		flEyePitch; // 0x7C
	float		flGoalFeetYaw; // 0x80
	float		flLastFeetYaw; // 0x84
	float		flMoveYaw; // 0x88
	float		flLastMoveYaw; // 0x8C // changes when moving/jumping/hitting ground
	float		flLeanAmount; // 0x90
	float		flLowerBodyYawAlignTime; // 0x94
	float		flFeetCycle; // 0x98 0 to 1
	float		flMoveWeight; // 0x9C 0 to 1
	float		flMoveWeightSmoothed; // 0xA0
	float		flDuckAmount; // 0xA4
	float		flHitGroundCycle; // 0xA8
	float		flRecrouchWeight; // 0xAC
	Vector		vecOrigin; // 0xB0
	Vector		vecLastOrigin;// 0xBC
	Vector		vecVelocity; // 0xC8
	Vector		vecVelocityNormalized; // 0xD4
	Vector		vecVelocityNormalizedNonZero; // 0xE0
	float		flVelocityLenght2D; // 0xEC
	float		flJumpFallVelocity; // 0xF0
	float		flSpeedNormalized; // 0xF4 // clamped velocity from 0 to 1 
	float		flRunningSpeed; // 0xF8
	float		flDuckingSpeed; // 0xFC
	float		flDurationMoving; // 0x100
	float		flDurationStill; // 0x104
	bool		bOnGround; // 0x108
	bool		bHitGroundAnimation; // 0x109
	std::byte	pad2[0x2]; // 0x10A
	float		flNextLowerBodyYawUpdateTime; // 0x10C
	float		flDurationInAir; // 0x110
	float		flLeftGroundHeight; // 0x114
	float		flHitGroundWeight; // 0x118 // from 0 to 1, is 1 when standing
	float		flWalkToRunTransition; // 0x11C // from 0 to 1, doesnt change when walking or crouching, only running
	std::byte	pad3[0x4]; // 0x120
	float		flAffectedFraction; // 0x124 // affected while jumping and running, or when just jumping, 0 to 1
	std::byte	pad4[0x208]; // 0x128
	float		flMinBodyYaw; // 0x330
	float		flMaxBodyYaw; // 0x334
	float		flMinPitch; //0x338
	float		flMaxPitch; // 0x33C
	int			iAnimsetVersion; // 0x340
}; // Size: 0x344

class anim_state {
public:
	char	u0[0x18];
	float	anim_update_timer;
	char	u1[0xC];
	float	started_moving_time;
	float	last_move_time;
	char	u2[0x10];
	float	last_lby_time;
	char	u3[0x8];
	float	run_amount;
	char	u4[0x10];
	void* entity;
	__int32 active_weapon;
	__int32 last_active_weapon;
	float	last_clientside_anim_update_time;
	__int32 last_clientside_anim_update_framecount;
	float	eye_timer;
	float	eye_angles_y;
	float	eye_angles_x;
	float	goal_feet_yaw;
	float	current_feet_yaw;
	float	torso_yaw;
	float	last_move_yaw;
	float	lean_amount;
	char	u5[0x4];
	float	feet_cycle;
	float	feet_yaw_rate;
	char	u6[0x4];
	float	duck_amount;
	float	landing_duck_amount;
	char	u7[0x4];
	Vector	current_origin;
	Vector	last_origin;
	float	velocity_x;
	float	velocity_y;
	char	u8[0x10];
	float	move_direction_1;
	float	move_direction_2;
	char	u9[0x4];
	float	velocity;
	float	jump_fall_velocity;
	float	clamped_velocity;
	float	feet_speed_forwards_or_sideways;
	float	feet_speed_unknown_forwards_or_sideways;
	float	last_time_started_moving;
	float	last_time_stopped_moving;
	bool	on_ground;
	bool	hit_in_ground_anim;
	char	u10[0x4];
	float	last_origin_z;
	float	head_from_ground_distance_standing;
	float	stop_to_full_running_fraction;
	char	u11[0x14];
	__int32 is_not_moving;
	char	u12[0x20];
	float	last_anim_update_time;
	float	moving_direction_x;
	float	moving_direction_y;
	float	moving_direction_z;
	char	u13[0x44];
	__int32 started_moving;
	char	u14[0x8];
	float	lean_yaw;
	char	u15[0x8];
	float	poses_speed;
	char	u16[0x8];
	float	ladder_speed;
	char	u17[0x8];
	float	ladder_yaw;
	char	u18[0x8];
	float	some_pose;
	char	u19[0x14];
	float	body_yaw;
	char	u20[0x8];
	float	body_pitch;
	char	u21[0x8];
	float	death_yaw;
	char	u22[0x8];
	float	stand;
	char	u23[0x8];
	float	jump_fall;
	char	u24[0x8];
	float	aim_blend_stand_idle;
	char	u25[0x8];
	float	aim_blend_crouch_idle;
	char	u26[0x8];
	float	strafe_yaw;
	char	u27[0x8];
	float	aim_blend_stand_walk;
	char	u28[0x8];
	float	aim_blend_stand_run;
	char	u29[0x8];
	float	aim_blend_crouch_walk;
	char	u30[0x8];
	float	move_blend_walk;
	char	u31[0x8];
	float	move_blend_run;
	char	u32[0x8];
	float	move_blend_crouch;
	char	u33[0x4];
	float	speed;
	__int32 moving_in_any_direction;
	float	acceleration;
	char	u34[0x74];
	float	crouch_height;
	__int32 is_full_crouched;
	char	u35[0x4];
	float	velocity_subtract_x;
	float	velocity_subtract_y;
	float	velocity_subtract_z;
	float	standing_head_height;
};
