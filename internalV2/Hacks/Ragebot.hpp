#pragma once
class Aimbot
{
private: // optimization
	// Restrict bad Times
	int NumScan = 0;			// number of scans done
	std::chrono::steady_clock::time_point StartTime;
	float DeltaTime;
	void TimerStart();
	bool TimerStop();
private: // essential var/funcs
	// var
	std::deque< EHitboxIndex> Priority;
	std::deque< EHitboxIndex> Hitboxes;
	float MinHitchance;
	int MinDamage;
	float HeadPointScale;
	float BodyPointScale;
	std::vector<std::pair<int, float>> players;
	// func
	void SortPlayers(std::vector<std::pair<int, float>>& values);
	void UpdateVals();
	float CalculateHitchance(Vector vangles, const Vector& point, Entity* player, int hbox);
	float CalculatePsudoHitchance(Entity* ent, int Hitbox);
	float GetPointScale(int Hitbox);
public:
	int TheoreticalScans;
	void Run();
	void RenderStats();
};

extern Aimbot* rage;

/*
NOTES: this prioritizes a given player, and their backtrack, then given players,
then their backtrack

basically it scans a player + their backtrack before scanning elsewhere
*/