#include "../Include.hpp"

Movement* movement = new Movement();

void Movement::Bunnyhop()
{
	if (!cfg->movement.Bunnyhop) return;

	static int HopsMade = 0;
		
	// if we don't hold jump key and we are on the ground, then reset hops made
	if (!(G::cmd->iButtons & IN_JUMP) && G::LocalplayerAlive && (G::Localplayer->GetFlags() & FL_ONGROUND))
		HopsMade = -1;

	bool MakeHop = true;

	// deal with bhop chance
	if (cfg->movement.Chance != 100) {
		// 0 - 100
		MakeHop = (rand() % 100) < cfg->movement.Chance;
	}

	// make sure we make the minimum jumps
	if (cfg->movement.MinHops != 0 && HopsMade < cfg->movement.MinHops)
		MakeHop = true;
	 
	// make sure we aren't exceeding the maximum jumps
	if (cfg->movement.MaxHops != 0 && HopsMade >= cfg->movement.MaxHops)
		MakeHop = false;

	// if we aren't supposed to make the hop... force the person not to jump
	if (!MakeHop && G::Localplayer->GetFlags() & FL_ONGROUND && (G::cmd->iButtons & IN_JUMP)) {
		G::cmd->iButtons &= ~IN_JUMP;
		HopsMade = -1;
		return;
	}
		
	// if we are on a different tick and we are allowed to bhop...
	if ((G::cmd->iButtons & IN_JUMP) && G::LocalplayerAlive && !(G::Localplayer->GetFlags() & FL_ONGROUND)
		&& G::Localplayer->GetMoveType() != MOVETYPE_LADDER) {
		G::cmd->iButtons &= ~IN_JUMP;
	}

	// if we got here, that means that we are currently doing a bhop, so increase HopsMade
	if (G::cmd->iButtons & IN_JUMP)
		HopsMade++;
}

void Movement::AutoStrafe()
{
	if (!cfg->movement.Autostrafe) return;

	int movetype = G::Localplayer->GetMoveType();
	if (movetype == MOVETYPE_LADDER || movetype == MOVETYPE_NOCLIP) //-V807
		return;

	if (G::Localplayer->GetFlags() & FL_ONGROUND)
		return;

	// force update only every tick
	/*static int OldTickCount = I::globalvars->m_tickCount;
	if (OldTickCount == I::globalvars->m_tickCount)
		return;
	OldTickCount = I::globalvars->m_tickCount;*/

	// Velocity direction
	auto GetVelDir = []() {
		Vector Vel = G::Localplayer->GetVecVelocity();
		return NormalizeYaw(RAD2DEG(atan2(Vel.y, Vel.x)));
	};

	// Goal direction
	auto GetGoalDir = []() {
		// which direction we want to go
		// (straight forward for now)
		
		Vector DirVec = Vector(G::cmd->flSideMove, G::cmd->flForwardMove, 0);
		float Dir = RAD2DEG(atan2(DirVec.x, DirVec.y));
		return NormalizeYaw(G::StartAngle.y - Dir);
	};

	// move direction
	auto GetMoveDirection = [](float VelDir, float GoalDir, bool InTransition)
	{
		float delta = NormalizeYaw(GoalDir - VelDir);
		int rotate = delta > 0 ? 1 : -1;
		return VelDir + ((90 + (InTransition ? cfg->movement.AngSacrifice : 0)) * rotate);
	};

	// get forward/sidemove from where we wanna go
	auto GetForwardSidemove = [](float MoveDir)
	{
		float delta = NormalizeYaw(G::StartAngle.y - MoveDir);
		float sin_rot = sinf(DEG2RAD(delta)) * 450.f;
		float cos_rot = cosf(DEG2RAD(delta)) * 450.f;
		return Vector(sin_rot, cos_rot, 0);
	};

	float VelDir = GetVelDir();
	float GoalDir = GetGoalDir();

	float delta = NormalizeYaw(GoalDir - VelDir);
	// if we need to transition...
	if (fabsf(delta) > (cfg->movement.StrafeAng * 2) + 5)
	{
		float MoveDir = GetMoveDirection(VelDir, GoalDir, true);
		Vector ForwardSideMove = GetForwardSidemove(MoveDir);
		G::cmd->flSideMove = ForwardSideMove.x;
		G::cmd->flForwardMove = ForwardSideMove.y;
	}
	// do p100 autostrafe
	else
	{
		bool flip = G::cmd->iCommandNumber % 2;
		GoalDir += flip ? cfg->movement.StrafeAng : -cfg->movement.StrafeAng;

		float MoveDir = GetMoveDirection(VelDir, GoalDir, false);
		Vector ForwardSideMove = GetForwardSidemove(MoveDir);

		G::cmd->flSideMove = ForwardSideMove.x;
		G::cmd->flForwardMove = ForwardSideMove.y;
		/*G::cmd->viewangles.y += flip ? -5 : 5;*/
	}
}

void Movement::FastStop()
{
	int movetype = G::Localplayer->GetMoveType();
	if (movetype == MOVETYPE_NOCLIP || movetype == MOVETYPE_LADDER || !(G::Localplayer->GetFlags() & 1) || G::cmd->iButtons & IN_JUMP)
		return;

	/*if (G::cmd->buttons & (IN_MOVELEFT | IN_MOVERIGHT | IN_FORWARD | IN_BACK))
		return;*/

	Vector velocity = G::Localplayer->GetVecVelocity();
	const float speed = velocity.VecLength2D();
	if (speed < 15.0f)
		return;

	Vector direction = Vector(0, RAD2DEG(atan2(velocity.y, velocity.x)), 0);

	//direction.y now holds delta between current viewangle, and velocity angle
	direction.y = G::cmd->angViewAngle.y - direction.y;

	//direction now holds forward/sidemove to achieve velocity angle (mentioned above)
	float SIN = sinf(DEG2RAD(direction.y)) * 450;
	float COS = cosf(DEG2RAD(direction.y)) * 450;
	direction.x = COS;
	direction.y = SIN;

	Vector negated_direction = direction * -speed;
	G::cmd->flForwardMove = negated_direction.x;
	G::cmd->flSideMove = negated_direction.y;
}

void Movement::SlowWalk()
{
	if (!cfg->Keybinds["Slowwalk"].boolean) return;

	Vector velocity = G::Localplayer->GetVecVelocity();
	const float speed = velocity.VecLength2D();
	if (speed < 15.0f)
		return;

	// valid to slowwalk check
	int movetype = G::Localplayer->GetMoveType();
	if (movetype == MOVETYPE_NOCLIP || movetype == MOVETYPE_LADDER || !(G::Localplayer->GetFlags() & 1) || G::cmd->iButtons & IN_JUMP)
		return;

	// Do some big brain smoothing
	float maxSpeed = G::Localplayer->MaxAccurateSpeed() / 3.f;
	maxSpeed -= 5;
	maxSpeed *= (cfg->movement.SlowWalkSpeed / 100.f);

	if (G::cmd->flForwardMove && G::cmd->flSideMove) {
		const float maxSpeedRoot = maxSpeed * static_cast<float>(M_SQRT1_2);
		G::cmd->flForwardMove = G::cmd->flForwardMove < 0.0f ? -maxSpeedRoot : maxSpeedRoot;
		G::cmd->flSideMove = G::cmd->flSideMove < 0.0f ? -maxSpeedRoot : maxSpeedRoot;
	}
	else if (G::cmd->flForwardMove) {
		G::cmd->flForwardMove = G::cmd->flForwardMove < 0.0f ? -maxSpeed : maxSpeed;
	}
	else if (G::cmd->flSideMove) {
		G::cmd->flSideMove = G::cmd->flSideMove < 0.0f ? -maxSpeed : maxSpeed;
	}

	// get some stuff
	Entity* weapon = G::Localplayer->GetActiveWeapon();
	if (!weapon) return;

	CCSWeaponData* data = weapon->GetWeaponData();
	if (!data) return;

	// big calculations
	auto get_standing_accuracy = [&]() -> const float
	{
		const auto max_speed = G::Localplayer->IsScoped() ? data->flMaxSpeed[1] : data->flMaxSpeed[0];
		return max_speed / 3.f;
	};

	// if going slow enough, chill
	if (speed < get_standing_accuracy())
		return;

	// STOP UR GOING WAY TO FAST MISTER
	FastStop();
}

void Movement::CM_Clamp()
{
	//Angle stuff
	if ((G::cmd->angViewAngle).x > 89) {
		(G::cmd->angViewAngle).x = 89;
	}
	else if ((G::cmd->angViewAngle).x < -89) {
		(G::cmd->angViewAngle).x = -89;
	}

	while ((G::cmd->angViewAngle).y > 180) {
		(G::cmd->angViewAngle).y -= 360;
	}
	while ((G::cmd->angViewAngle).y < -180) {
		(G::cmd->angViewAngle).y += 360;
	}
	//(G::cmd->angViewAngle).z = 0;

	//side/forwardMove/upmove
	if (G::cmd->flSideMove > 450.f) {
		G::cmd->flSideMove = 450.f;
	}
	if (G::cmd->flSideMove < -450.f) {
		G::cmd->flSideMove = -450.f;
	}
	if (G::cmd->flForwardMove > 450.f) {
		G::cmd->flForwardMove = 450.f;
	}
	if (G::cmd->flForwardMove < -450.f) {
		G::cmd->flForwardMove = -450.f;
	}
	if (G::cmd->flUpMove > 320.f) {
		G::cmd->flUpMove = 320.f;
	}
	if (G::cmd->flUpMove < -320.f) {
		G::cmd->flUpMove = -320.f;
	}
}

void Movement::CM_Start(CUserCmd* cmd, bool* pSendPacket)
{
	G::StartAngle = cmd->angViewAngle;
	G::cmd = cmd;
	G::pSendpacket = pSendPacket;
}

void Movement::CM_MoveFixStart()
{
	G::StartForwardmove = G::cmd->flForwardMove;
	G::StartSidemove = G::cmd->flSideMove;
}

void Movement::CM_MoveFixEnd()
{
	//fix movement
	static float deltaView;
	static float f1;
	static float f2;

	if (G::StartAngle.y < 0.f)
		f1 = 360.0f + G::StartAngle.y;
	else
		f1 = G::StartAngle.y;

	if (G::cmd->angViewAngle.y < 0.0f)
		f2 = 360.0f + G::cmd->angViewAngle.y;
	else
		f2 = G::cmd->angViewAngle.y;

	if (f2 < f1)
		deltaView = abs(f2 - f1);
	else
		deltaView = 360.0f - abs(f1 - f2);

	deltaView = 360.0f - deltaView;

	G::cmd->flForwardMove = cos(DEG2RAD(deltaView)) * G::StartForwardmove + cos(DEG2RAD(deltaView + 90.f)) * G::StartSidemove;
	G::cmd->flSideMove = sin(DEG2RAD(deltaView)) * G::StartForwardmove + sin(DEG2RAD(deltaView + 90.f)) * G::StartSidemove;

	/*
	IN_FORWARD
		IN_BACK
		IN_MOVELEFT
		IN_MOVERIGHT
		*/

		/*if (G::cmd->forwardmove > 0)
		{
			G::cmd->buttons |= IN_FORWARD;
			G::cmd->buttons &= ~IN_BACK;
		}
		if (G::cmd->forwardmove < 0)
		{
			G::cmd->buttons |= IN_BACK;
			G::cmd->buttons &= ~IN_FORWARD;
		}*/
}

void Movement::CM_End()
{
	// save the z
	float z = G::cmd->angViewAngle.z;
	G::cmd->angViewAngle.NormalizeAngle(); //prevent csgo from hating us
	G::cmd->angViewAngle.z = z;
	CM_Clamp();
	G::EndAngle.x = G::cmd->angViewAngle.x;
	G::EndAngle.y = G::cmd->angViewAngle.y;
	G::EndAngle.z = G::cmd->angViewAngle.z;


	/*if (*G::pSendPacket) {
		G::FakeAngle = G::cmd->viewangles;
		G::Localplayer->SetupBones(G::FakeMatrix, 128, 0x100, I::globalvars->m_curTime);
	}
	else
		G::RealAngle = G::cmd->viewangles;*/

	
}
