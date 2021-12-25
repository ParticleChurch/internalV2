#pragma once
struct BoneStruct
{
	bool Valid = false;
	Vector Head;
	Vector Pelvis;
	Vector Stomach;
	Vector Chest;
};

class LagComp
{
public:
	struct Player;
	struct Record;

	struct Record
	{
		Entity* pEntity; // parent entity
		matrix3x4_t Matrix[MAXSTUDIOBONES];
		BoneStruct bones;
		Vector Origin;
		Vector HeadPos;
		float SimulationTime;
		Vector Velocity;
		int Flags;

		void Update(Entity* ent);
	};

	struct Player
	{
		Entity* pEntity;
		bool Dormant;
		int LifeState;
		int Health;
		int Team;
		int Index;
		PlayerInfo_t Info;
		std::deque<LagComp::Record> records;

		// resolver crap
		int Shots = 0; // shots made on player in general
		int ShotsMissed = 0;
	};

private:
	// Adds / updates player (NO REMOVALS, only updates
	void Update(Entity* ent, PlayerInfo_t* info);
	// cleans up playerlist to remove crappy records / players
	void CleanUp();
	// Deal with lagging...
	Vector oAbsOrigin;
	int* render;
	int backup;
	void Backup(Entity* ent);
	void Restore(Entity* ent);
	void Extrapolate(Entity* ent);
public:
	// clears/removes all records (basically if spectating)
	void ClearRecords();
	// clears/removes all players from player list (including records)
	void ClearPlayerList();
public:
	std::map<int, Player> PlayerList;
	void Run_FSN(int stage);
	void RenderStats();
};

extern LagComp* lagcomp;
