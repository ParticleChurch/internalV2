#pragma once
class IVDebugOverlay
{
protected:
	~IVDebugOverlay() = default;
public:
	virtual void AddEntityTextOverlay(int ent_index, int line_offset, float duration, int r, int g, int b, int a, const char* format, ...) = 0;
	virtual void AddBoxOverlay(const Vector& origin, const Vector& mins, const Vector& max, Vector const& orientation, int r, int g, int b, int a, float duration) = 0;
	virtual void AddSphereOverlay(const Vector& origin, float radius, int theta, int phi, int r, int g, int b, int a, float duration) = 0;
	virtual void AddTriangleOverlay(const Vector& p1, const Vector& p2, const Vector& p3, int r, int g, int b, int a, bool no_depth_test, float duration) = 0;
private:
	virtual void a() = 0;
public:
	virtual void AddLineOverlay(const Vector& origin, const Vector& dest, int r, int g, int b, bool no_depth_test, float duration) = 0;

	void AddCapsuleOverlay(Vector& mins, Vector& maxs, float pillradius, int r, int g, int b, int a, float duration)
	{
		typedef void(__thiscall* oAddCapsuleOverlay)(void*, Vector&, Vector&, float&, int, int, int, int, float);
		return GetVFunc<oAddCapsuleOverlay>(this, 24)(this, mins, maxs, pillradius, r, g, b, a, duration);

	}

};