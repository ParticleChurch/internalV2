#pragma once

/*
Record is kinda janky, currently holds
both a matrix, as well as bones, idk,
lotta data but c++ is fast 
*/

class LagComp
{
public:
	struct Player;
	struct Record;

	struct Record
	{
		Entity* pEntity; // parent entity
		matrix3x4_t Matrix[MAXSTUDIOBONES];
		bool ValidBones = false;
		mstudiobbox_t bones[19];
		Vector Origin;
		Vector HeadPos;
		float SimulationTime;
		Vector Velocity;
		int Flags;
		bool extrapolated = false;

		void Update(Entity* ent);
		void Extrapolate(Entity* ent);
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
