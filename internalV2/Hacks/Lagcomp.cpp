#include "../Include.hpp"

LagComp* lagcomp = new LagComp();

#define DEBUG_LAGCOMP true 

void LagComp::Record::Extrapolate(Entity* ent, int UserID)
{
	return;
	// dont bother if no lag
	if (this->LagAmount < 1) return;

	float velocity = ent->GetVecVelocity().VecLength2D();
	H::console.push_back("velocity: " + std::to_string(velocity));
	if (velocity < 1.f)
		return;

	// tick delta (you can predict forward 3-4 ticks and hit them still lol)
	int tick_delta = this->LagAmount;
	H::console.push_back("tick_delta: " + std::to_string(tick_delta));

	// if shit tick delta
	/*if (tick_delta > 15 && tick_delta < 2)
		return;*/

	// set extrapolated flag
	this->extrapolated = true;

	// change the simulated time to corrected one
	this->SimulationTime += tick_delta * I::globalvars->flIntervalPerTick;

	// Move the position for all of the bones...
	H::console.push_back("_______________");
	H::console.push_back(this->HeadPos.str());
	Vector bef = Calc::ExtrapolateTick(this->HeadPos, ent->GetVecVelocity(), tick_delta);
	H::console.push_back(std::to_string((bef - this->HeadPos).VecLength()));
	this->HeadPos = bef;
	H::console.push_back(this->HeadPos.str());

	/*
	for (auto a : this->bones)
	{
		a.vecBBMin = Calc::ExtrapolateTick(a.vecBBMin, this->Velocity, tick_delta);
		a.vecBBMax = Calc::ExtrapolateTick(a.vecBBMax, this->Velocity, tick_delta);
	}

	if constexpr (DEBUG_LAGCOMP) L::Debug("Model Check");
	Model_t* model = ent->GetModel();
	if (!model)
		return;

	if constexpr (DEBUG_LAGCOMP) L::Debug("StudioModel Check");
	studiohdr_t* StudioModel = I::modelinfo->GetStudioModel(model);
	if (!StudioModel)
		return;

	// IDK Y I HAV TO PUT THIS HERE BUT IF I DONT IT SOMETIMES JUST CRASHES. FML
	if (I::globalvars->flCurrentTime < 10.f)
		return;

	if constexpr (DEBUG_LAGCOMP) L::Debug("move bone positions");
	for (int i = 0; i < StudioModel->nBones; i++)
	{
		if constexpr (DEBUG_LAGCOMP) L::Debug(std::to_string(i).c_str());
		mstudiobone_t* Bone = StudioModel->GetBone(i);
		if (!Bone ) //|| !(Bone->iFlags & 256) || Bone->iParent == -1
			continue;

		if (i >= MAXSTUDIOBONES) continue;

		// get current bone position
		Vector Pos = Vector(this->Matrix[i][0][3], this->Matrix[i][1][3], this->Matrix[i][2][3]);

		// get predicted bone position
		Vector NextPos = Calc::ExtrapolateTick(Pos, this->Velocity, tick_delta);

		// set the values
		this->Matrix[i].arrData[0][3] = NextPos.x;
		this->Matrix[i].arrData[1][3] = NextPos.y;
		this->Matrix[i].arrData[2][3] = NextPos.z;
	}
*/
}

// kinda counter intuative, this interpolates (if valid tick, everything behind it xD)
void LagComp::Record::Interpolate(Entity* ent, int UserID)
{

}

void LagComp::Record::Update(Entity* ent, int UserID)
{
	float latency = (I::engine->GetNetChannelInfo()->GetAvgLatency(0) + I::engine->GetNetChannelInfo()->GetAvgLatency(1));

	if constexpr (DEBUG_LAGCOMP) L::Debug("SETUP BONES");
	bool FixBones = ent->GetVecVelocity().Length2D() > 1.f && !ent->IsDormant() && ent->GetSimulationTime() == ent->GetOldSimulationTime();
	if (ent->SetupBones(this->Matrix, MAXSTUDIOBONES, 0x100, 0, false))
	{
		this->HeadPos = Vector(this->Matrix[8][0][3], this->Matrix[8][1][3], this->Matrix[8][2][3]);

		Model_t* model = ent->GetModel();
		if (model) {
			studiohdr_t* StudioModel = I::modelinfo->GetStudioModel(model);
			if (StudioModel) {
				this->ValidBones = true;

				static std::vector< int> hitboxes = { 
					HITBOX_HEAD,
					HITBOX_NECK,
					HITBOX_PELVIS,
					HITBOX_STOMACH,
					HITBOX_THORAX,
					HITBOX_CHEST,
					HITBOX_UPPER_CHEST,
					HITBOX_RIGHT_THIGH,
					HITBOX_LEFT_THIGH,
					HITBOX_RIGHT_CALF,
					HITBOX_LEFT_CALF,
					HITBOX_RIGHT_FOOT,
					HITBOX_LEFT_FOOT,
					HITBOX_RIGHT_HAND,
					HITBOX_LEFT_HAND,
					HITBOX_RIGHT_UPPER_ARM,
					HITBOX_RIGHT_FOREARM,
					HITBOX_LEFT_UPPER_ARM,
					HITBOX_LEFT_FOREARM,
				};

				for (auto HITBOX : hitboxes)
				{
					mstudiobbox_t* StudioBox = StudioModel->GetHitboxSet(0)->GetHitbox(HITBOX);
					if (StudioBox) std::memcpy(&this->bones[HITBOX], StudioBox, sizeof(mstudiobbox_t));
				}
			}
			else
				this->ValidBones = false;
		}
		else
			this->ValidBones = false;
	}
	else
		this->ValidBones = false;
	if constexpr (DEBUG_LAGCOMP) L::Debug("GetMaxDesyncDelta");
	this->MaxDesync = ent->GetMaxDesyncDelta();
	if constexpr (DEBUG_LAGCOMP) L::Debug("GetVecOrigin");
	this->Origin = ent->GetVecOrigin();
	if constexpr (DEBUG_LAGCOMP) L::Debug("GetSimulationTime");
	this->SimulationTime = ent->GetSimulationTime() + latency;
	if constexpr (DEBUG_LAGCOMP) L::Debug("GetVecVelocity");
	this->Velocity = ent->GetVecVelocity();
	if constexpr (DEBUG_LAGCOMP) L::Debug("GetVecVelocity");
	this->Flags = ent->GetFlags();
	if constexpr (DEBUG_LAGCOMP) L::Debug("LagAmount");
	this->LagAmount = abs(TimeToTicks(ent->GetSimulationTime() - ent->GetOldSimulationTime()));

	// NO CRAZY SHIT BRUGH
	/*if constexpr (DEBUG_LAGCOMP) L::Debug("Extrapolate");
	this->Extrapolate(ent, UserID);*/
	/*if constexpr (DEBUG_LAGCOMP) L::Debug("Extrapolate");
	Interpolate(ent, UserID);*/
}

void LagComp::Update(Entity* ent, PlayerInfo_t* info)
{
	if (!info->szName) return;

	if constexpr (DEBUG_LAGCOMP) L::Debug(("LagComp Update: " + (std::string)info->szName).c_str(), "\n");

	// if player doesn't exist --> add it
	if (PlayerList.find(info->nUserID) == PlayerList.end())
	{
		LagComp::Player NewPlayer;
		NewPlayer.badShots = 0;
		PlayerList.insert(std::pair(info->nUserID, NewPlayer));
	}

	// Now we update the player
	Player& player = PlayerList[info->nUserID];

	if constexpr (DEBUG_LAGCOMP) L::Debug("Getting basic info");


	// update player vars
	player.pEntity = ent;
	player.Dormant = ent->IsDormant();
	player.LifeState = ent->GetLifeState();
	player.Health = ent->GetHealth();
	player.Team = ent->GetTeam();
	player.Index = ent->GetIndex();

	

	if constexpr (DEBUG_LAGCOMP) L::Debug("copy name");
	std::memcpy(&player.Info, &info, sizeof(PlayerInfo_t));

	// animation goofery
	bool validatingtick = player.Dormant || player.LifeState != LIFE_ALIVE;
	memcpy(player.OldAnimLayers, validatingtick ? ent->GetAnimOverlays() : player.AnimLayers, 15 * sizeof CAnimationLayer);
	memcpy(player.AnimLayers, ent->GetAnimOverlays(), 15 * sizeof CAnimationLayer);


	// add record
	LagComp::Record NewRec;
	NewRec.Update(ent, info->nUserID);
	if constexpr (DEBUG_LAGCOMP) L::Debug("player.records.push_front");
	player.records.push_front(NewRec);

	if constexpr (DEBUG_LAGCOMP) L::Debug("player updated");
}

void LagComp::CleanUp()
{
	if (PlayerList.empty()) return;

	if constexpr (DEBUG_LAGCOMP) L::Debug("lagcomp->CleanUp()");

	auto CleanAllRecords = [](int userid) {
		if constexpr (DEBUG_LAGCOMP) L::Debug("lagcomp->CleanAllRecords()");

		// return false if there is no records for that
		if constexpr (DEBUG_LAGCOMP) L::Debug("return false if there is no records for that");
		if (lagcomp->PlayerList.find(userid) == lagcomp->PlayerList.end())
			return false;

		// don't bother clearing if the records are already empty
		if constexpr (DEBUG_LAGCOMP) L::Debug("don't bother clearing if the records are already empty");
		if (lagcomp->PlayerList[userid].records.empty())
			return true;

		// otherwise clear records and return true
		if constexpr (DEBUG_LAGCOMP) L::Debug("otherwise clear records and return true");
		lagcomp->PlayerList[userid].records.clear();
		lagcomp->PlayerList[userid].records.resize(0);
		return true;
	};

	auto CleanAndDeletePlayer = [](int userid) {
		if constexpr (DEBUG_LAGCOMP) L::Debug(("lagcomp->CleanAndDeletePlayer() " + std::to_string(userid)).c_str());

		// make sure this fucker isn't emtpy
		if constexpr (DEBUG_LAGCOMP) L::Debug("CleanAndDeletePlayer->empty check");
		if (lagcomp->PlayerList.empty()) return false;

		// return false if there is no records for that
		if constexpr (DEBUG_LAGCOMP) L::Debug("CleanAndDeletePlayer->PlayerList find check");
		if (lagcomp->PlayerList.find(userid) == lagcomp->PlayerList.end())
			return false;

		// otherwise clear records and erase the item, then return true
		if constexpr (DEBUG_LAGCOMP) L::Debug("lagcomp->clear()");
		lagcomp->PlayerList[userid].records.clear();

		if constexpr (DEBUG_LAGCOMP) L::Debug("lagcomp->resize()");
		lagcomp->PlayerList[userid].records.resize(0);

		if constexpr (DEBUG_LAGCOMP) L::Debug("lagcomp->erase()");
		lagcomp->PlayerList.erase(lagcomp->PlayerList.find(userid)); // remove current item

		if constexpr (DEBUG_LAGCOMP) L::Debug("lagcomp->cleananddelete return");
		return true;
	};

	PlayerInfo_t info;

	// go through all the players + records, and make sure they are valid
	for (auto& item : PlayerList)
	{
		if constexpr (DEBUG_LAGCOMP) L::Debug("lagcomp-> player = item.second;");
		Player& player = item.second;

		// remove player if entity is nullptr
		if (!player.pEntity)
		{
			CleanAndDeletePlayer(item.first);
			continue;
		}

		// remove player if entity is no longer human
		if (!I::engine->GetPlayerInfo(player.Index, &info)) {
			CleanAndDeletePlayer(item.first);
			continue;
		}

		// remove all records if entity is dormant
		if (player.Dormant) CleanAllRecords(item.first);

		// remove all records if bad entity lifestate
		if (player.LifeState != LIFE_ALIVE) CleanAllRecords(item.first);

		// remove all records if entitys health isn't greater than zero
		if (!(player.Health > 0)) CleanAllRecords(item.first);

		// remove player if entity is on same team as localplayers
		if (player.Team == G::LocalplayerTeam)
		{
			CleanAndDeletePlayer(item.first);
			continue;
		}

		// remove player if entity index is localplayers
		if (player.Index == G::LocalplayerIndex) {
			CleanAndDeletePlayer(item.first);

			continue;
		}

		// remove player if current (front) entity simulation time is super bad (aka > 1)
		if (!player.records.empty() && fabsf(I::globalvars->flCurrentTime - player.records.front().SimulationTime) > 1) 
		{
			CleanAndDeletePlayer(item.first);
			continue;
		}

		// remove bad RECORDS (not Player)
		while (player.records.size() > 3
			&& fabsf(I::globalvars->flCurrentTime - player.records.back().SimulationTime) > 0.2f) { // 700ms big brain is what it should be (max 2x breaklagcomp ~500ms)
			player.records.pop_back();
		}
	}
}

void LagComp::ClearRecords()
{
	if (PlayerList.empty()) return;

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
	//FRAME_NET_UPDATE_POSTDATAUPDATE_START FRAME_RENDER_START
	if (stage != FRAME_NET_UPDATE_POSTDATAUPDATE_START) return;

	// don't bother updating if not valid localplayer
	if (!G::Localplayer) return;
	if (!G::LocalplayerAlive) return;

	if constexpr (DEBUG_LAGCOMP) L::Debug("FrameStageNotifyHook LagComp", "\n");

	Entity* ent;
	for (int i = 1; i < I::engine->GetMaxClients(); ++i)
	{
		// Localplayer Check
		if (i == G::LocalplayerIndex) continue;

		// entity existence check
		ent = I::entitylist->GetClientEntity(i);
		if (!ent) continue;

		// entity player check
		if (!ent->IsPlayer()) continue;

		// just do enemies for now
		if (ent->GetTeam() == G::LocalplayerTeam) continue;

		// do another player check
		PlayerInfo_t info;
		if (!I::engine->GetPlayerInfo(i, &info))
			continue;

		// Update based on info
		Update(ent, &info);
	}

	// Clean up records
	CleanUp();
}

void LagComp::RenderStats()
{
	// go through all the players + records, and make sure they are valid
	ImGui::Begin("Lagcomp Records");
	if (!PlayerList.empty())
	{
		int i = 0;
		for (auto& item : PlayerList)
		{
			i++;
			Player& player = item.second;

			std::string str = std::to_string(i) + ") UserID: " + std::to_string(item.first) /*+ " Name: " + player.Info.szName*/ + " Dormant: " + (player.Dormant ? "True" : "False") + " Health: " + std::to_string(player.Health);
			ImGui::Text(str.c_str());
		}
	}
	ImGui::End();
}
