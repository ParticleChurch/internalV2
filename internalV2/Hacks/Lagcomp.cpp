#include "../Include.hpp"

LagComp* lagcomp = new LagComp();

void LagComp::Record::Update(Entity* ent)
{
	// STUPID BUG FUCK IT IMMA JUST FORCE: spec_replay_autostart 0 <-- force no death cam
	L::Debug("SETUP BONES");
	if (ent->SetupBones(this->Matrix, MAXSTUDIOBONES, 0x100, 0, false))
		this->HeadPos = Vector(this->Matrix[8][0][3], this->Matrix[8][1][3], this->Matrix[8][2][3]);

	L::Debug("GetVecOrigin");
	this->Origin = ent->GetVecOrigin();
	L::Debug("GetSimulationTime");
	this->SimulationTime = ent->GetSimulationTime();
	L::Debug("GetVecVelocity");
	this->Velocity = ent->GetVecVelocity();
	L::Debug("GetVecVelocity");
	this->Flags = ent->GetFlags();
}

void LagComp::Update(Entity* ent, int index)
{
	// Get info, otherwise don't even bother trying to update if not human
	PlayerInfo_t info;
	if (!I::engine->GetPlayerInfo(index, &info))
		return;

	L::Debug(("LagComp Update: " + (std::string)info.szName).c_str(), "\n");

	// if player doesn't exist --> add it
	if (PlayerList.find(info.nUserID) == PlayerList.end())
	{
		LagComp::Player NewPlayer;
		PlayerList.insert(std::pair(info.nUserID, NewPlayer));
	}

	// Now we update the player
	Player& player = PlayerList[info.nUserID];

	// update player vars
	player.pEntity = ent;
	player.Dormant = ent->IsDormant();
	player.LifeState = ent->GetLifeState();
	player.Health = ent->GetHealth();
	player.Team = ent->GetTeam();
	player.Index = index;
	std::memcpy(&player.Info, &info, sizeof(PlayerInfo_t));

	// add record
	LagComp::Record NewRec;
	NewRec.Update(ent);
	player.records.push_front(NewRec);
}

void LagComp::CleanUp()
{
	auto CleanAllRecords = [](int userid) {
		// return false if there is no records for that
		if (lagcomp->PlayerList.find(userid) == lagcomp->PlayerList.end())
			return false;

		// don't bother clearing if the records are already empty
		if (lagcomp->PlayerList[userid].records.empty())
			return true;

		// otherwise clear records and return true
		lagcomp->PlayerList[userid].records.clear();
		lagcomp->PlayerList[userid].records.resize(0);
		return true;
	};

	PlayerInfo_t info;

	// go through all the players + records, and make sure they are valid
	for (auto& item : PlayerList)
	{
		Player& player = item.second;

		// remove all records if entity is nullptr
		if (!player.pEntity) CleanAllRecords(item.first);

		// remove all records if entity is no longer human
		if (!I::engine->GetPlayerInfo(player.Index, &info)) CleanAllRecords(item.first);

		// remove all records if entity is dormant
		if (player.Dormant) CleanAllRecords(item.first);

		// remove all records if bad entity lifestate
		if (player.LifeState != LIFE_ALIVE) CleanAllRecords(item.first);

		// remove all records if entitys health isn't greater than zero
		if (!(player.Health > 0)) CleanAllRecords(item.first);

		// remove all records if entity is on same team as localplayers
		if (player.Team == G::LocalplayerTeam) CleanAllRecords(item.first);

		// remove all records if entity index is localplayers
		if (player.Index == G::LocalplayerIndex) CleanAllRecords(item.first);

		// remove remaining bad records
		while (player.records.size() > 3
			&& fabsf(I::globalvars->flCurrentTime - player.records.back().SimulationTime) > 1) {
			player.records.pop_back();
		}
	}
}

void LagComp::ClearRecords()
{
	// clear all the records (whether valid or not)
	for (auto& item : PlayerList)
	{
		item.second.records.clear();
	}
}

void LagComp::ClearPlayerList()
{
	ClearRecords();
	PlayerList.clear();
}

void LagComp::Run_FSN(int stage)
{
	if (stage != FRAME_RENDER_START) return;

	// don't bother updating if not valid localplayer
	if (!G::Localplayer) return;
	if (!G::LocalplayerAlive) return;

	L::Debug("FrameStageNotifyHook LagComp", "\n");

	Entity* ent;
	for (int i = 1; i < I::engine->GetMaxClients(); ++i)
	{
		// Localplayer Check
		if (i == G::LocalplayerIndex) continue;

		// entity existence check
		ent = I::entitylist->GetClientEntity(i);
		if (!ent) continue;

		// just do enemies for now
		if (ent->GetTeam() == G::LocalplayerTeam) continue;

		// Update based on info
		Update(ent, i);
	}

	// Clean up records
	CleanUp();
}