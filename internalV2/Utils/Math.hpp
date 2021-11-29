#pragma once

//Abusive macros for dealing with radian/degree conversion
#define RAD2DEG(x) ((x) * 180.0 / M_PI )
#define DEG2RAD(x) ((x) * M_PI / 180.0 )

inline float NormalizeYaw(float a)
{
	while (a > 180)
		a -= 360;
	while (a < -180)
		a += 360;
	return a;
}