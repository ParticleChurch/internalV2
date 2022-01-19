#include "../Include.hpp"

Thirdperson* thirdperson = new Thirdperson();

Vector Thirdperson::GetIdealCameraPos(float distance)
{
	Vector PlayerPos = G::Localplayer->GetEyePosition();	//pleyer center position
	Vector Ideal = PlayerPos;							//Final ideal angle
	Vector FPAng = Vector(G::StartAngle.x, G::StartAngle.y);				//flipped player angle
	FPAng.x *= -1;
	FPAng.y += 180;
	FPAng.NormalizeAngle();

	Ideal.z += distance * sin(DEG2RAD(FPAng.x));
	float Hz = distance * cos(DEG2RAD(FPAng.x)); //horizonal distance

	Ideal.x += Hz * cos(DEG2RAD(FPAng.y));
	Ideal.y += Hz * sin(DEG2RAD(FPAng.y));

	return Ideal;
}

float Thirdperson::GetCameraBoomLength(float distance)
{
	Vector IdealCameraPos = GetIdealCameraPos(distance);	//ideal camera position

	Vector PlayerPos = G::Localplayer->GetEyePosition();

	// first trace inwards

	/*MASK_VISIBLE*/
	Trace_t Trace;
	Ray_t Ray(PlayerPos, IdealCameraPos);
	CTraceFilter Filter((IHandleEntity*)G::Localplayer);
	I::enginetrace->TraceRay(Ray, MASK_SHOT_HULL, &Filter, &Trace); // originaly mask all

	float ReturnDist = distance * Trace.flFraction;

	int NumTimes = 0;
	PlayerInfo_t info;

	// surely we don't deal with more than 3 fucking idiots
	while (Trace.pHitEntity && I::engine->GetPlayerInfo(Trace.pHitEntity->GetIndex(), &info) && NumTimes <= 10)
	{
		NumTimes++;
		CTraceFilter NewFilter((IHandleEntity*)Trace.pHitEntity);
		Ray_t NewRay(Trace.vecEnd, IdealCameraPos);
		I::enginetrace->TraceRay(NewRay, MASK_SHOT_HULL, &NewFilter, &Trace); // originaly mask all

		ReturnDist += Trace.flFraction * (Trace.vecEnd - Trace.vecStart).VecLength();
	}

	return ReturnDist * 0.9f;
}

void Thirdperson::Run_hkCamToFirstPeron()
{
	if (cfg->Keybinds["Thirdperson"].boolean && G::Localplayer && G::LocalplayerAlive)
		return;
	H::console.clear();
	H::console.resize(0);
	H::console.push_back("CAMTOFIRSTPERSON");
	H::oCamToFirstPeron(I::input);
}

//enum ObserverMode {
//	OBS_MODE_IN_EYE = 4,
//	OBS_MODE_CHASE,
//	OBS_MODE_ROAMING = 7
//};

void Thirdperson::Run_DoPostScreenEffects()
{
	static bool isInThirdperson{ false };

	if (I::engine->IsInGame() && G::Localplayer && G::LocalplayerAlive) {
		if (cfg->Keybinds["Thirdperson"].boolean)
		{
			isInThirdperson = true;
			I::input->m_fCameraInThirdPerson = true;
			I::input->m_vecCameraOffset = Vector(G::StartAngle.x, G::StartAngle.y, GetCameraBoomLength(cfg->vis.ThirdPersonDistance));
		}
		else
		{
			isInThirdperson = false;
			I::input->m_fCameraInThirdPerson = false;
			I::input->m_vecCameraOffset = Vector(G::StartAngle.x, G::StartAngle.y, 0);
		}
	}
}

/*
void Thirdperson()
{
	if (!interfaces.engine->IsInGame() || !csgo->local)
		return;

	//    static float cur_dist = vars.visuals.thirdperson_dist;

	if (!vars.misc.thirdperson.active)
	{
		enabledtp = false;
		//cur_dist = vars.visuals.thirdperson_dist;
		return;
	}

	if (csgo->local->isAlive() && enabledtp && csgo->client_state->iDeltaTick > 0)
	{
		if (!interfaces.input->m_fCameraInThirdPerson)
			interfaces.input->m_fCameraInThirdPerson = true;

		if (interfaces.input->m_fCameraInThirdPerson)
		{
			//if (cur_dist > 0.f)
			//	cur_dist -= 1.f * sin(animation_speed * 10.f);

			auto distance = vars.visuals.thirdperson_dist;//- cur_dist;

			Vector angles;
			interfaces.engine->GetViewAngles(angles);

			Vector inverse_angles;
			interfaces.engine->GetViewAngles(inverse_angles);

			inverse_angles.z = distance;

			Vector forward, right, up;
			Math::AngleVector4(inverse_angles, forward, right, up);

			Ray_t ray;
			CTraceFilterWorldAndPropsOnly filter;
			trace_t trace;

			auto eye_pos = csgo->fake_duck
				? csgo->local->GetRenderOrigin() + interfaces.game_movement->GetPlayerViewOffset(false)
				: csgo->local->GetRenderOrigin() + csgo->local->GetVecViewOffset();
			auto offset = eye_pos + forward * -distance + right + up;

			ray.Init(eye_pos, offset, Vector(-16.0f, -16.0f, -16.0f), Vector(16.0f, 16.0f, 16.0f));
			interfaces.trace->TraceRay(ray, MASK_SHOT_HULL, &filter, &trace);

			trace.fraction = clamp(trace.fraction, 0.f, 1.f);

			angles.z = distance * trace.fraction;

			interfaces.input->m_vecCameraOffset = angles;
		}
		//else
		//	cur_dist = vars.visuals.thirdperson_dist;
	}
	else {
		//cur_dist = vars.visuals.thirdperson_dist;
		interfaces.input->m_fCameraInThirdPerson = false;
		interfaces.input->m_vecCameraOffset.z = 0.f;
	}

	static auto b_once = false;

	if (csgo->local->isAlive())
	{
		b_once = false;
		return;
	}

	if (b_once)
		csgo->local->SetObserverMode(5);

	if (csgo->local->GetObserverMode() == 4)
		b_once = true;
}
*/