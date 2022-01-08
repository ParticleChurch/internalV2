#include "../Include.hpp"

AntiAim* antiaim = new AntiAim();

#define DEBUG_AA true

bool AntiAim::DistanceBreaker()
{
	// If our next position breaks it...(64 units) (64*64 = 4096)
	if ((NextPos - PrevPos).VecLength() > max(TrigDistance, 4096.f))
		return true;
	return false;
}

bool AntiAim::TimeBreaker()
{
	// we can't choke more than 62 ticks because of some protection (prev exploits)
	// which is around 16 packets (a more accurate way would be to count ticks, meh)
	if (I::engine->GetNetChannelInfo()->chokedPackets > LagAmount)
		return true;
	return false;
}

void AntiAim::LagOnPeak()
{
	if (!cfg->fakelag.LagOnPeak) return;

	if (!G::Localplayer)
		return;
	if (!G::LocalplayerAlive)
		return;

	// no point in lagging if velocity is too low
	if (int(G::Localplayer->GetVecVelocity().VecLength2D()) < 100)
	{
		LaggingOnPeak = false;
		return;
	}

	bool DamageOutgoing = false;

	// create psudo point for lp to be
	Vector newPos = Calc::ExtrapolateTick(G::Localplayer->GetEyePosition(), G::Localplayer->GetVecVelocity(), 8);

	std::map<int, LagComp::Player>::iterator it;
	for (it = lagcomp->PlayerList.begin(); it != lagcomp->PlayerList.end(); it++)
	{
		if (it->second.Index == G::LocalplayerIndex) // entity is Localplayer
			continue;

		if (!(it->second.pEntity)) // entity DOES NOT exist
			continue;

		if (!(it->second.Health > 0)) // entity is NOT alive
			continue;

		if (it->second.Team == G::LocalplayerTeam) // Entity is on same team
			continue;

		if (it->second.Dormant)	// Entity is dormant
			continue;

		/*if (autowall2->CanHitFloatingPoint(it->second.EyePos, NextPos))
		{

			H::console.push_back("BOOM HITTIN A POINT!");
			DamageOutgoing = true;
			break;
		}*/
		if (autowall->GetDamage(newPos, G::Localplayer, it->second.pEntity->GetEyePosition()) > 0)
		{
			DamageOutgoing = true;
			/*break;*/
		}
		// if we are going to scan too many people, skip
		/*if (scans > aimbot->maxplayerscan / 3)
			continue;*/
	}
	
	L::Debug("Fakelag autowall - end");

	static bool ChokeOnce = false;

	// If there is damage incoming, we are about to send packets, and we have not choked before
	if (DamageOutgoing && PredictedVal && !ChokeOnce)
	{
		ChokeOnce = true;				// We have choked once
		LaggingOnPeak = true;
	}
	else if (!DamageOutgoing && ChokeOnce) // no damage incoming, reset the choke once
	{
		ChokeOnce = false;
	}

	// we've done the lag
	if (PredictedVal && !ChokeOnce)
		LaggingOnPeak = false;

	// If there we should lag on peak... 
	if (LaggingOnPeak && ChokeOnce)
	{
		
		TrigDistance = (64.f * 64.f * 0.9f);
		TrigTicks = 14;
		LagAmount = TrigTicks;
	}
}

#define HEADEDGE 30.f
float AntiAim::GetYawBase()
{
	if (cfg->aa.YawBase == 0)
	{
		return NormalizeYaw(G::StartAngle.y + 180);
	}
	// at crosshair
	else if (cfg->aa.YawBase == 1)
	{
		float BestCrossDist = FLT_MAX;
		float BestAng = G::StartAngle.y;
		for (auto& a : lagcomp->PlayerList)
		{
			if (!(a.second.Health > 0)) continue;

			Vector Ang = Calc::CalculateAngle(a.second.pEntity->GetVecOrigin());
			float Cross = Calc::CrosshairDist(Ang);
			if (Cross < BestCrossDist)
			{
				BestCrossDist = Cross;
				BestAng = Ang.y;
			}
		}
		return NormalizeYaw(BestAng + 180);
	}
	// "freestanding"
	else if (cfg->aa.YawBase == 2)
	{
		QAngle angle;
		Vector position = G::Localplayer->GetEyePosition();

		float closest_distance = 100.0f;

		float radius = HEADEDGE + 0.1f;
		float step = (float)M_PI * 2.f / 8.f;

		for (float a = 0; a < (M_PI * 2.0); a += step)
		{
			Vector location(radius * cos(a) + position.x, radius * sin(a) + position.y, position.z);

			Trace_t tr;
			Ray_t ray(position, location);
			CTraceFilter traceFilter((IHandleEntity*)G::Localplayer);
			//I::enginetrace->TraceRay(ray, 0x4600400B, &traceFilter, &tr);
			I::enginetrace->TraceRay(ray, MASK_SHOT, &traceFilter, &tr);

			Vector delta = position - tr.vecEnd;

			float distance = delta.VecLength();

			if (distance < closest_distance)
			{
				closest_distance = distance;
				angle.y = RAD2DEG(a);
			}
		}

		if (closest_distance < HEADEDGE)
		{
			return NormalizeYaw(angle.y);
		}
		else
		{
			return NormalizeYaw(G::StartAngle.y + 180);
		}
	}
	else
	{
		return NormalizeYaw(G::StartAngle.y + 180);
	}
}

float RandomVal(float min, float max)
{
	int Delta = max - min;
	if (Delta == 0)
		return 0.f;
	float val = rand() % Delta;
	return min + val;
}

void AntiAim::Rage()
{
	if constexpr (DEBUG_AA) L::Debug("START RAGE");

	// skip if no localplayer
	if (!G::Localplayer) return;

	// skip if localplayer not alive
	if (!G::LocalplayerAlive) return;

	// skip if shitty sendpacket ptr
	if (!G::pSendpacket) return;

	// skip if on ladder or nocliping
	int movetype = G::Localplayer->GetMoveType();
	if (movetype == MOVETYPE_LADDER || movetype == MOVETYPE_NOCLIP) return;

	// force down syndrome
	G::cmd->angViewAngle.x = 89;

	// get what side we are on
	bool side = cfg->Keybinds["Invert AA"].boolean;

	/*H::console.clear();
	H::console.resize(0);*/

	// Deal with Yawbase
	float YawBase = GetYawBase();
	YawBase = NormalizeYaw(YawBase + (side ? cfg->aa.LeftBaseOffset : cfg->aa.RightBaseOffset));


	/*H::console.push_back(std::to_string(G::StartAngle.y));
	H::console.push_back(std::to_string(G::cmd->angViewAngle.y));
	H::console.push_back(std::to_string(YawBase));*/

	if constexpr (DEBUG_AA) L::Debug("MAX DESYNC");

	float MaxDesync = G::Localplayer->GetMaxDesyncDelta();

	// init crap
	float DesyncOffset; // static/opposite
	static float CurSway = 0.f; // sway
	static bool SwaySide = true; // sway
	static float CurSpin = YawBase;	// spin
	static bool JitterSide = true; // jitter
	static float Jitter_OldTime = I::globalvars->flCurrentTime;
	static float Random_OldTime = I::globalvars->flCurrentTime; // random
	static float Random_Val = 0.f;


	if constexpr (DEBUG_AA) L::Debug("OTHER SHIT");

	// massive huge brain switch statement
	switch (cfg->aa.AAType)
	{
	case 0: // static
		DesyncOffset = min(MaxDesync, side ? cfg->aa.LeftLimit : cfg->aa.RightLimit);
		G::cmd->angViewAngle.y = YawBase;
		if (*G::pSendpacket)
		{
			G::cmd->angViewAngle.y += side ? -DesyncOffset : DesyncOffset;
		}
		break;
	case 1: // sway
		G::cmd->angViewAngle.y = YawBase;
		// add / subtract based on sway step, and current side
		CurSway += SwaySide ? cfg->aa.SwayStep : -cfg->aa.SwayStep;

		// deal with limits
		DesyncOffset = min(G::Localplayer->GetMaxDesyncDelta(), (CurSway > 0 ? cfg->aa.LeftLimit : cfg->aa.RightLimit));
		if (CurSway < -DesyncOffset)
		{
			CurSway = -DesyncOffset;
			SwaySide = !SwaySide;
		}
		if (CurSway > DesyncOffset)
		{
			CurSway = DesyncOffset;
			SwaySide = !SwaySide;
		}

		G::cmd->angViewAngle.y = YawBase;
		if (*G::pSendpacket)
		{
			G::cmd->angViewAngle.y += CurSway;
		}
		break;
	case 2: // opposite
		DesyncOffset = min(MaxDesync, side ? cfg->aa.LeftLimit : cfg->aa.RightLimit);
		DesyncOffset *= side ? -1 : 1;
		G::cmd->angViewAngle.y = YawBase - DesyncOffset / 2;
		if (*G::pSendpacket)
		{
			G::cmd->angViewAngle.y += DesyncOffset;
		}
		break;
	case 3: // jitter (like opposite, just jitting 0.2
		if (fabsf(I::globalvars->flCurrentTime - Jitter_OldTime) > 0.22)
		{
			JitterSide = !JitterSide;
			Jitter_OldTime = I::globalvars->flCurrentTime;
		}

		DesyncOffset = min(MaxDesync, side ? cfg->aa.LeftLimit : cfg->aa.RightLimit);
		DesyncOffset *= JitterSide ? -1 : 1;
		G::cmd->angViewAngle.y = YawBase - DesyncOffset / 2;
		if (*G::pSendpacket)
		{
			G::cmd->angViewAngle.y += DesyncOffset;
		}
		break;
	case 4: // random (truely fucking stupid)
		{
			float max = min(MaxDesync, Random_Val < 0 ? cfg->aa.LeftLimit : cfg->aa.RightLimit);
			if (fabsf(I::globalvars->flCurrentTime - Random_OldTime) > 0.22)
			{
				Random_OldTime = I::globalvars->flCurrentTime;
				Random_Val = RandomVal(-max, max);
			}

			// make do fake do weird shit
			if (*G::pSendpacket)
			{
				G::cmd->angViewAngle.y = YawBase + Random_Val;
			}
			// make real do real fucking weird shit
			else
			{
				// make real do weird shit
				float RealRandAng = YawBase + Random_Val;
				if (Random_Val < 0)
					RealRandAng += max;
				else
					RealRandAng -= max;
				G::cmd->angViewAngle.y = RealRandAng;
			}
				
		}
		break;
	case 5: // spin
		// do the real part
		CurSpin += side ? cfg->aa.SpinStep : -cfg->aa.SpinStep;
		G::cmd->angViewAngle.y = NormalizeYaw(CurSpin);

		// do the fake part ( it follows the real part )
		DesyncOffset = MaxDesync;
		if (*G::pSendpacket)
		{
			G::cmd->angViewAngle.y += side ? -DesyncOffset : DesyncOffset;
		}

		break;
	}

	// lean that Thang
	if(*G::pSendpacket)
		G::cmd->angViewAngle.z = cfg->aa.LeanAngle;

	G::cmd->angViewAngle.y = NormalizeYaw(G::cmd->angViewAngle.y);

	//H::console.push_back(std::to_string(G::cmd->angViewAngle.y));

	// update fake and real lol
	if (*G::pSendpacket)
		G::fake = Vector(G::cmd->angViewAngle.x, G::cmd->angViewAngle.y, G::cmd->angViewAngle.z);
	else
		G::real = Vector(G::cmd->angViewAngle.x, G::cmd->angViewAngle.y, G::cmd->angViewAngle.z);
}

void AntiAim::Legit()
{
	// skip if no localplayer
	if (!G::Localplayer) return;

	// skip if localplayer not alive
	if (!G::LocalplayerAlive) return;

	// skip if shitty sendpacket ptr
	if (!G::pSendpacket) return;

	// skip if on ladder or nocliping
	int movetype = G::Localplayer->GetMoveType();
	if (movetype == MOVETYPE_LADDER || movetype == MOVETYPE_NOCLIP) return;

	float MaxDesync = G::Localplayer->GetMaxDesyncDelta();

	float DesyncOffset = cfg->aa.LegitAmount; 
	std::clamp(DesyncOffset, -MaxDesync, MaxDesync);

	// lean bitch
	if (*G::pSendpacket)
		G::cmd->angViewAngle.z = cfg->aa.LeanAngle;

	// desync bitch
	if (!*G::pSendpacket)
	{
		G::cmd->angViewAngle.y += DesyncOffset;
	}

	// update fake and real lol
	if (*G::pSendpacket)
		G::fake = Vector(G::cmd->angViewAngle.x, G::cmd->angViewAngle.y, G::cmd->angViewAngle.z);
	else
		G::real = Vector(G::cmd->angViewAngle.x, G::cmd->angViewAngle.y, G::cmd->angViewAngle.z);
}

void AntiAim::RunMovement()
{
	// if no anti-aim, return
	if(!cfg->aa.Enable && !cfg->aa.LegitAA)
		return;

	// If the localplayer is not alive...
	if (!G::Localplayer || !G::LocalplayerAlive)
		return;

	// If not on ground
	if (!(G::Localplayer->GetFlags() & FL_ONGROUND)) return;

	// if we aren't moving enough, MOVE
	if (fabsf(G::cmd->flSideMove) < 5.0f) {
		G::cmd->flSideMove = G::cmd->iTickCount & 1 ? 3.25f : -3.25f;
	}
}

void AntiAim::Run()
{
	if (cfg->aa.Enable) {
		Rage();
		return;
	}
	else if (cfg->aa.LegitAA)
	{
		Legit();
		return;
	}
	else
	{
		G::real = Vector(G::cmd->angViewAngle.x, G::cmd->angViewAngle.y);
		G::fake = Vector(G::cmd->angViewAngle.x, G::cmd->angViewAngle.y);
		return;
	}	
}

void AntiAim::FakelagStart()
{
	// fakelag exception
	if (cfg->Keybinds["Fake Duck"].boolean)
	{
		LagAmount = 14;
		PredictedVal = I::engine->GetNetChannelInfo()->chokedPackets >= LagAmount;
		return;
	}

	// only update every tick
	static int oldTickCount = I::globalvars->iTickCount;
	if (oldTickCount == I::globalvars->iTickCount) return;
	oldTickCount = I::globalvars->iTickCount;

	// Update Lag Amount every 200 ms
	static float oldTime = I::globalvars->flCurrentTime;
	if (fabsf(oldTime - I::globalvars->flCurrentTime) > 0.2)
	{
		oldTime = I::globalvars->flCurrentTime;

		LagAmount = cfg->fakelag.LagTicks;

		// random variable between [-RandomTicks/2] and [RandomTicks/2]
		if (cfg->fakelag.RandomTicks > 0)
			LagAmount += ((rand() % cfg->fakelag.RandomTicks) - (cfg->fakelag.RandomTicks / 2));

		// force it to be proper
		LagAmount = min(14, LagAmount);
	}

	// Update Velocity
	Vector velocity = G::Localplayer->GetVecVelocity();
	velocity *= (I::globalvars->flIntervalPerTick * 2);
	//velocity now holds our distance to move (doubled for safety)

	// Updating our next position
	NextPos = G::Localplayer->GetEyePosition() + velocity;


	// If breaking distance or time --> Send packets
	bool distancebreak = DistanceBreaker();
	bool timebreak = TimeBreaker();

	// Updating Limits if trigger 
	LagOnPeak();

	PredictedVal = timebreak || distancebreak;

	if (PredictedVal)
		PrevPos = G::Localplayer->GetEyePosition();
}

bool AntiAim::FakelagEnd()
{
	return PredictedVal;
}

void AntiAim::Visualize()
{
	if (!cfg->aa.Enable) return;
	if (!G::Localplayer) return;
	if (!G::LocalplayerAlive) return;


	int xSize;
	int ySize;
	I::engine->GetScreenSize(xSize, ySize);

	static DWORD FONT = I::surface->FontCreate();
	static bool Once = true;
	if (Once)
	{
		Once = false;
		I::surface->SetFontGlyphSet(FONT, "Verdana", 12, 1, 0, 0, FONTFLAG_DROPSHADOW);// FONTFLAG_ANTIALIAS | FONTFLAG_OUTLINE);
	}

	I::surface->DrawSetTextFont(FONT);

	Vector sPoint1;
	Vector sPoint2;
	Vector origin = G::Localplayer->GetVecOrigin();

	// DRAW THE REAL
	float DeltaAng = G::real.y;
	float yDelta = 20 * sinf(DEG2RAD(DeltaAng));
	float xDelta = 20 * cosf(DEG2RAD(DeltaAng));
	Vector point = Vector(origin.x + xDelta, origin.y + yDelta, origin.z);
	if (WorldToScreen(origin, sPoint1) && WorldToScreen(point, sPoint2))
	{
		I::surface->DrawSetColor(Color(255.f, 255.f, 255.f, 255.f));
		I::surface->DrawLine(sPoint1.x, sPoint1.y, sPoint2.x, sPoint2.y);

		I::surface->DrawSetTextColor(Color(255.f, 255.f, 255.f, 255.f));
		I::surface->DrawSetTextPos(sPoint2.x, max(0, sPoint2.y - 15));
		I::surface->DrawPrintText(L"real", wcslen(L"real"));
	}

	// DRAW THE FAKE
	DeltaAng = G::fake.y;
	yDelta = 20 * sinf(DEG2RAD(DeltaAng));
	xDelta = 20 * cosf(DEG2RAD(DeltaAng));
	point = Vector(origin.x + xDelta, origin.y + yDelta, origin.z);
	if (WorldToScreen(origin, sPoint1) && WorldToScreen(point, sPoint2))
	{
		I::surface->DrawSetColor(Color(255.f, 0, 0, 255.f));
		I::surface->DrawLine(sPoint1.x, sPoint1.y, sPoint2.x, sPoint2.y);

		I::surface->DrawSetTextColor(Color(255.f, 0, 0, 255.f));
		I::surface->DrawSetTextPos(sPoint2.x, max(0, sPoint2.y - 15));
		I::surface->DrawPrintText(L"fake", wcslen(L"fake"));
	}

	// DRAW THE LAG
	point = Vector(PrevPos.x, PrevPos.y, origin.z);
	if (this->LaggingOnPeak && WorldToScreen(origin, sPoint1) && WorldToScreen(point, sPoint2))
	{
		I::surface->DrawSetColor(Color(0.f, 255.f, 0, 255.f));
		I::surface->DrawLine(sPoint1.x, sPoint1.y, sPoint2.x, sPoint2.y);
	}
}
