#pragma once
class Aimbot
{
private: // VAR
// optimization
	int MaxScan = 20;			// max number of times we can scan (per tick)
	int NumScan = 0;			// number of scans so far
	float DeltaTime = 0.f;		// how long it took for us to scan
	bool ScanTimeBad = false;

	// functions for managing the number of scans
	void TimerStop(std::chrono::steady_clock::time_point start);
	float CalculateHitchance(Vector vangles, const Vector& point, Entity* player, int hbox);
	float CalculatePsudoHitchance();
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