#pragma once
class Aimbot
{
private: // VAR
// optimization
	int MaxScan = 20;			// max number of times we can scan (per tick)
	int NumScan = 0;			// number of scans so far
	float DeltaTime = 0.f;		// how long it took for us to scan
	bool ScanTimeBad = false;

	// factoids
	bool BacktrackShot = false;	// was that a backtrack shot?
	Vector AimPoint;
	int AimTickCount;
	int AimUserID;
	matrix3x4_t AimMatrix[128];
	float MaxHitchance = 0.f;
	float MaxDamage = 0.f;

	// For logging
	/*AimbotShot shot;*/

	// Aimbot var
	float MinHitchance = 60.f;
	float MinDamage = 60.f;
	float HeadScale = 0.f;
	float BodyScale = 0.f;
	float LimbScale = 0.f;
	std::deque<EHitboxIndex> hitboxes = { };
	std::deque<EHitboxIndex> priority = {};
private:
	void UpdateHitboxes();
	float GetPointScale(int Hitbox);
private: // FUNC
	float CalculateHitchance(Vector vangles, const Vector& point, Entity* player, int hbox);
	float CalculatePsudoHitchance();
	bool ValidPlayer(LagComp::Player player);

	// Sorts UserID's of players in an order that makes sense for autowalling
	std::vector<std::pair<int, float>> players;
	void GetPlayers(std::vector<std::pair<int, float>>& values);
	// Scans through players and attempts to autowall them 
	bool ScanPlayers();
	// Scans a given player and attempts to autowall them 
	bool ScanPlayer(int UserID);
	// Scans a given player backtrack and attempts to autowall them
	bool ScanPlayerBacktrack(int UserID);
	// Scans through and entities head safepoint (if any)
	bool GetSafepoint(Entity* ent, int UserID, Vector point, float radius, Vector& safepoint);

	// RAGE FUNC
	void Rage();

public: // FUNC
	void Run();
	void RenderStats();
};

extern Aimbot* aimbot;

/*
NOTES: this prioritizes a given player, and their backtrack, then given players,
then their backtrack

basically it scans a player + their backtrack before scanning elsewhere
*/