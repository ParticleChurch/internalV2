#pragma once

//Abusive macros for dealing with radian/degree conversion
#define RAD2DEG(x) ((x) * 180.0 / M_PI )
#define DEG2RAD(x) ((x) * M_PI / 180.0 )

namespace Calc
{
	static Vector CalculateAngle(Vector Target, Vector Source)
	{
		static float Distance;	//Total distance between target and starting head position
		Distance = sqrtf(powf(Source.x - Target.x, 2) + powf(Source.y - Target.y, 2) + powf(Source.z - Target.z, 2));

		static float Height;	//total difference in height between target and starting head position
		Height = Source.z - Target.z;

		static Vector Angle;
		Angle.x = RAD2DEG(asin(-Height / Distance));								//pitch angle
		Angle.y = RAD2DEG(atan2(Source.y - Target.y, Source.x - Target.x));			//yaw angle

		Angle.NormalizeAngle();	//making sure the angles are proper
		return Angle;
	}

	static Vector CalculateAngle(Vector Target)
	{
		Vector EyePos = G::Localplayer->GetEyePosition();//starting head position
		return CalculateAngle(EyePos, Target);
	}

	static float CrosshairDist(Vector TargetAngle)
	{
		static Vector cur;
		cur = { G::StartAngle.x, G::StartAngle.y,0 };
		cur.NormalizeAngle();
		cur += G::Localplayer->GetAimPunchAngle();
		cur.NormalizeAngle();

		//turn to 0-360 degrees
		TargetAngle.y += 180;
		cur.y += 180;

		//finds both angles and chooses shortest yaw angle delta
		float a = fabsf(TargetAngle.y - cur.y);
		float b = 360 - fabsf(TargetAngle.y - cur.y);

		//gets the pitch angle delta 
		float c = abs(TargetAngle.x - cur.x);

		//gets the total distance (in degrees)
		return sqrtf(powf(min(a, b), 2) + powf(c, 2));
	}
}

static float GetLerp()
{
	static auto cl_interp = I::convar->FindVar("cl_interp");
	static auto cl_updaterate = I::convar->FindVar("cl_updaterate");
	const auto update_rate = cl_updaterate->GetInt();
	const auto interp_ratio = cl_interp->GetFloat();

	auto lerp = interp_ratio / update_rate;

	if (lerp <= interp_ratio)
		lerp = interp_ratio;

	return lerp;
}

static int TimeToTicks(float time) noexcept
{
	return static_cast<int>(0.5f + time / I::globalvars->flIntervalPerTick);
}

static float TicksToTime(int ticks) {
	return I::globalvars->flIntervalPerTick * (float)(ticks);
}

static bool WorldToScreen(Vector& in, Vector& out)
{
	auto matrix = I::engine->WorldToScreenMatrix();

	float w = matrix[3][0] * in.x + matrix[3][1] * in.y + matrix[3][2] * in.z + matrix[3][3];

	if (w > 0.001f) {
		static int width = -1;
		static int height = -1;
		I::surface->GetScreenSize(width, height);
		out.x = width / 2 * (1 + (matrix[0][0] * in.x + matrix[0][1] * in.y + matrix[0][2] * in.z + matrix[0][3]) / w);
		out.y = height / 2 * (1 - (matrix[1][0] * in.x + matrix[1][1] * in.y + matrix[1][2] * in.z + matrix[1][3]) / w);
		out.z = 0.0f;

		return true;
	}
	return false;
}

inline float DotProduct(const float* v1, const float* v2) {
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

inline float DotProduct(Vector v1, Vector v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline float NormalizeYaw(float a)
{
	while (a > 180)
		a -= 360;
	while (a < -180)
		a += 360;
	return a;
}