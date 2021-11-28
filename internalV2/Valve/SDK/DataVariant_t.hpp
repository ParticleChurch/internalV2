#pragma once
struct DataVariant_t
{
	union
	{
		float	Float;
		long	Int;
		char* String;
		void* Data;
		float	Vector[3];
		int64_t Int64;
	};

	ESendPropType iType;
};