#pragma once

class AntiAim
{
public:
	// TEMP VARS FOR NOW
	QAngle fake;
	QAngle real;
public:
	// lag vars
	bool PredictedVal = true;
	float DistPrevPos; // 64 units
	Vector PrevPos;
	Vector NextPos;
	int LagAmount = 1;
private:
	// AA vars
	float AtCrossTarg;
	float AtDistTarg;
	float AtWallTarg;
private:
	// lag funcs
	bool DistanceBreaker();
	bool TimeBreaker();
	void LagOnPeak();

	// Trigger Limits
	float TrigDistance;
	float TrigTicks;

private:
	// AA funcs
	float GetYawBase();
	void Rage();

	void Legit();

public:
	bool LaggingOnPeak = false;

	void RunMovement();
	void Run();
	void FakelagStart();
	bool FakelagEnd();

	void Visualize();
};

extern AntiAim* antiaim;

