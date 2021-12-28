#pragma once
class Aimbot
{
private: // optimization
	//var
	int MaxScan = 30;			// max number of times we can scan (per tick)
	int NumScan = 0;			// number of scans so far
	float DeltaTime = 0.f;		// how long it took for us to scan
	bool ScanTimeBad = false;

	// functions for managing the number of scans
	void TimerStop(std::chrono::steady_clock::time_point start);
private: // essential var/funcs
	// var
	std::deque< EHitboxIndex> Priority;
	float MinHitchance;
	int MinDamage;
	float HeadPointScale;
	float BodyPointScale;
	// func
	void UpdateVals();
	float CalculateHitchance(Vector vangles, const Vector& point, Entity* player, int hbox);
	float CalculatePsudoHitchance();
	float GetPointScale(int Hitbox);
public:
	void Run();
	void RenderStats();
};

extern Aimbot* aimbot;

/*
NOTES: this prioritizes a given player, and their backtrack, then given players,
then their backtrack

basically it scans a player + their backtrack before scanning elsewhere
*/