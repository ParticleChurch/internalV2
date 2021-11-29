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
