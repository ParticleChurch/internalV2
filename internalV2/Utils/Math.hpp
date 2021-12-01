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

inline void SinCos(float radians, float* sine, float* cosine)
{
	*sine = sin(radians);
	*cosine = cos(radians);
}

inline void AngleVectors(const Vector& angles, Vector* forward, Vector* right = nullptr, Vector* up = nullptr)
{
	float sr, sp, sy, cr, cp, cy;
	SinCos(DEG2RAD(angles.y), &sy, &cy);
	SinCos(DEG2RAD(angles.x), &sp, &cp);
	SinCos(DEG2RAD(angles.z), &sr, &cr);

	if (forward)
	{
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}

	if (right)
	{
		right->x = (-1 * sr * sp * cy + -1 * cr * -sy);
		right->y = (-1 * sr * sp * sy + -1 * cr * cy);
		right->z = -1 * sr * cp;
	}

	if (up)
	{
		up->x = (cr * sp * cy + -sr * -sy);
		up->y = (cr * sp * sy + -sr * cy);
		up->z = cr * cp;
	}
}

inline void fast_rsqrt(float a, float* out)
{
	const auto xx = _mm_load_ss(&a);
	auto xr = _mm_rsqrt_ss(xx);
	auto xt = _mm_mul_ss(xr, xr);
	xt = _mm_mul_ss(xt, xx);
	xt = _mm_sub_ss(_mm_set_ss(3.f), xt);
	xt = _mm_mul_ss(xt, _mm_set_ss(0.5f));
	xr = _mm_mul_ss(xr, xt);
	_mm_store_ss(out, xr);
}

static float fast_vec_normalize(Vector& vec)
{
	const auto sqrlen = vec.SquareLength() + 1.0e-10f;
	float invlen;
	fast_rsqrt(sqrlen, &invlen);
	vec.x *= invlen;
	vec.y *= invlen;
	vec.z *= invlen;
	return sqrlen * invlen;
}

inline float QuickRandom(float low, float high)
{
	return low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
}

inline void fast_sqrt(float* __restrict p_out, float* __restrict p_in)
{
	_mm_store_ss(p_out, _mm_sqrt_ss(_mm_load_ss(p_in)));
	// compiles to movss, sqrtss, movss
}

inline void vector_angles(const Vector& forward, Vector& angles)
{
	float tmp, yaw, pitch;

	if (forward[2] == 0.0f && forward[0] == 0.0f)
	{
		yaw = 0;

		if (forward[2] > 0.0f)
			pitch = 90.0f;
		else
			pitch = 270.0f;
	}
	else
	{
#ifdef QUICK_MATH
		yaw = (fast_atan2(forward[1], forward[0]) * 180.0f / M_PI);
#else
		yaw = (atan2f(forward[1], forward[0]) * 180.0f / M_PI);
#endif

		if (yaw < 0.0f)
			yaw += 360.0f;

		float sqin = forward[0] * forward[0] + forward[1] * forward[1];
		fast_sqrt(&tmp, &sqin);

#ifdef QUICK_MATH
		pitch = (fast_atan2(-forward[2], tmp) * 180.0f / M_PI);
#else
		pitch = (atan2f(-forward[2], tmp) * 180.0f / M_PI);
#endif

		if (pitch < 0.0f)
			pitch += 360.0f;
	}

	pitch -= floorf(pitch / 360.0f + 0.5f) * 360.0f;
	yaw -= floorf(yaw / 360.0f + 0.5f) * 360.0f;

	if (pitch > 89.0f)
		pitch = 89.0f;
	else if (pitch < -89.0f)
		pitch = -89.0f;

	angles.x = pitch;
	angles.y = yaw;
	angles.z = 0;
}

// Does the ray intersect the sphere
static bool RayIntersectingSphere(Vector center, float radius, Vector start, Vector end)
{
	Vector oc = start - center;
	//
	Vector direction = end - start;

	float a = DotProduct(direction, direction);
	float b = 2.0 * DotProduct(oc, direction);
	float c = DotProduct(oc, oc) - radius * radius;
	float discriminant = b * b - 4 * a * c;
	return (discriminant > 0);
}

static bool DoesRayIntersectHitbox(Vector min, Vector max, float radius, Vector start, Vector end)
{
	// does it intersect the bottom hitbox
	if (RayIntersectingSphere(min, radius, start, end))
		return true;

	// does it interset the top hitbox
	if (RayIntersectingSphere(max, radius, start, end))
		return true;

	// otherwise were gonna assume false, as typically we go for the tops/bottoms
	return false;

	// TO DO THIS PROPER CONSIDER something like: https://gist.github.com/jdryg/ecde24d34aa0ce2d4d87
}

// Returns whether or not two spheres (with identical radius) are intersecting
static bool IntersectingSpheres(float radius, Vector a, Vector b)
{
	float distance = (a - b).VecLength();
	if (distance > 2 * radius)
		return false;
	else
		return true;
}

/*
Returns whether or not a safepoint is found, and if it is changes safepoint Vec to a good point
start is the start of the ray
points are the various centers of spheres (expecting only 3)
radius is the radius of the spheres
*/
static bool FoundSafepoint(Vector start, std::vector<Vector> points, float radius, Vector& safepoint)
{
	// First go through the spheres and see if any intersect, if they do, then it is a GREAT safepoint
	int p1_index = 0;
	for (auto p1 : points)
	{
		int p2_index = 0;
		for (auto p2 : points)
		{
			// Don't wanna check if sphere intersects with itself lmao
			if (p1_index == p2_index)
				continue;

			// If the spheres are intersecting, that's a GREAT safepoint so...
			if (IntersectingSpheres(radius, p1, p2))
			{
				safepoint = (p1 + p2) / 2.f;
				return true; // return as a intersecting sphere safepoint is found
			}

			// if, while aiming for p2, we shoot a ray and hit sphere 1...
			if (RayIntersectingSphere(p1, radius, start, p2))
			{
				safepoint = p2; //p2 or p1 works, its just a point that, with our direction, we can aim at and hit both
				return true; // return as a intersecting sphere safepoint is found
			}

			p2_index++;
		}
		p1_index++;
	}
	return false;
}