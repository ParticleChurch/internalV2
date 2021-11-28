#pragma once
struct Ray_t
{
	Ray_t(const Vector& vecStart, const Vector& vecEnd) :
		vecStart(vecStart), vecDelta(vecEnd - vecStart), matWorldAxisTransform(nullptr), bIsRay(true)
	{
		this->bIsSwept = (this->vecDelta.LengthSqr() != 0.f);
	}

	Ray_t(const Vector& vecStart, const Vector& vecEnd, const Vector& vecMins, const Vector& vecMaxs)
	{
		this->vecDelta = vecEnd - vecStart;

		this->matWorldAxisTransform = nullptr;
		this->bIsSwept = (this->vecDelta.LengthSqr() != 0.f);

		this->vecExtents = vecMaxs - vecMins;
		this->vecExtents *= 0.5f;
		this->bIsRay = (this->vecExtents.LengthSqr() < 1e-6f);

		this->vecStartOffset = vecMins + vecMaxs;
		this->vecStartOffset *= 0.5f;
		this->vecStart = vecStart + this->vecStartOffset;
		this->vecStartOffset *= -1.0f;
	}

	VectorAligned		vecStart;
	VectorAligned		vecDelta;
	VectorAligned		vecStartOffset;
	VectorAligned		vecExtents;
	const matrix3x4_t* matWorldAxisTransform;
	bool				bIsRay;
	bool				bIsSwept;
};