#include "../Include.hpp"

LagComp* lagcomp = new LagComp();

#define DEBUG_LAGCOMP true 

void LagComp::Record::Update(Entity* ent)
{
	if constexpr (DEBUG_LAGCOMP) L::Debug("SETUP BONES");
	bool FixBones = ent->GetVecVelocity().Length2D() > 1.f;
	if (ent->SetupBones(this->Matrix, MAXSTUDIOBONES, 0x100, 0, false))
	{
		this->HeadPos = Vector(this->Matrix[8][0][3], this->Matrix[8][1][3], this->Matrix[8][2][3]);

		Model_t* model = ent->GetModel();
		if (model) {
			studiohdr_t* StudioModel = I::modelinfo->GetStudioModel(model);
			if (StudioModel) {
				this->bones.Valid = true;

				static std::vector< EHitboxIndex> hitboxes = { HITBOX_HEAD, HITBOX_PELVIS, HITBOX_STOMACH, HITBOX_UPPER_CHEST };

				for (auto& HITBOX : hitboxes)
				{
					mstudiobbox_t* StudioBox = StudioModel->GetHitboxSet(0)->GetHitbox(HITBOX_HEAD);
					if (!StudioBox) continue;

					Vector min = StudioBox->vecBBMin.Transform(this->Matrix[StudioBox->iBone]);
					Vector max = StudioBox->vecBBMax.Transform(this->Matrix[StudioBox->iBone]);

					Vector point = (min + max) / 2.f;

					// special p100 multipoint
					if (HITBOX == HITBOX_HEAD)
					{
						// go for upper echelons of hittin head
						point = min.z > max.z ? min : max;

						// arbitrary switch statement
						switch (G::cmd->iTickCount % 2)
						{
						case 0: // target top left
							point = ent->GetTopLeft(point, 0.45f, G::Localplayer);
							break;
						case 1: // target top right
							point = ent->GetTopRight(point, 0.45f, G::Localplayer);
							break;
						}
					}
					// lame multipoint
					else
					{
						switch (G::cmd->iTickCount % 2)
						{
						case 0: // target left
							point = ent->GetLeft(point, 0.7f, G::Localplayer);
							break;
						case 1: // target right
							point = ent->GetRight(point, 0.7f, G::Localplayer);
							break;
						}
					}

					// assign the points
					switch (HITBOX)
					{
					case HITBOX_HEAD:
						this->bones.Head = point;
						break;
					case HITBOX_PELVIS:
						this->bones.Pelvis = point;
						break;
					case HITBOX_STOMACH:
						this->bones.Stomach = point;
						break;
					case HITBOX_UPPER_CHEST:
						this->bones.Chest = point;
						break;
					default:
						break;
					}
				}
			}
			else
				this->bones.Valid = false;
		}
		else
			this->bones.Valid = false;
	}
	else
		this->bones.Valid = false;
		

	if constexpr (DEBUG_LAGCOMP) L::Debug("GetVecOrigin");
	this->Origin = ent->GetVecOrigin();
	if constexpr (DEBUG_LAGCOMP) L::Debug("GetSimulationTime");
	this->SimulationTime = ent->GetSimulationTime();
	if constexpr (DEBUG_LAGCOMP) L::Debug("GetVecVelocity");
	this->Velocity = ent->GetVecVelocity();
	if constexpr (DEBUG_LAGCOMP) L::Debug("GetVecVelocity");
	this->Flags = ent->GetFlags();
}

void LagComp::Update(Entity* ent, PlayerInfo_t* info)
{
	if (!info->szName) return;

	if constexpr (DEBUG_LAGCOMP) L::Debug(("LagComp Update: " + (std::string)info->szName).c_str(), "\n");

	// if player doesn't exist --> add it
	if (PlayerList.find(info->nUserID) == PlayerList.end())
	{
		LagComp::Player NewPlayer;
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

	// back up the entity position
	Backup(ent);

	// extrapolate if needed
	Extrapolate(ent);

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

	auto CleanAndDeletePlayer = [](int userid) {
		// return false if there is no records for that
		if (lagcomp->PlayerList.find(userid) == lagcomp->PlayerList.end())
			return false;
		// otherwise clear records and erase the item, then return true
		lagcomp->PlayerList[userid].records.clear();
		lagcomp->PlayerList[userid].records.resize(0);
		lagcomp->PlayerList.erase(lagcomp->PlayerList.find(userid)); // remove current item
		return true;
	};

	PlayerInfo_t info;

	// go through all the players + records, and make sure they are valid
	for (auto& item : PlayerList)
	{
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
			&& fabsf(I::globalvars->flCurrentTime - player.records.back().SimulationTime) > 0.2f) {
			player.records.pop_back();
		}
	}
}

void LagComp::Backup(Entity* ent)
{
	render = reinterpret_cast<int*>(ent + 0x274);
	this->backup = *render;

	this->oAbsOrigin = ent->GetAbsOrigin();

	*render = 0;
}

void LagComp::Restore(Entity* ent)
{
	ent->SetAbsOrigin(this->oAbsOrigin);

	*render = this->backup;

}

Vector ExtrapolateTick(Vector p0, Vector v0, int ticks = 1) {
	return p0 + (v0 * I::globalvars->flIntervalPerTick * ticks);
}

void LagComp::Extrapolate(Entity* ent)
{
	return;

	float velocity = ent->GetVecVelocity().Length2D();
	if (velocity > 3)
		return;

	// tick delta
	int tick_delta = abs(TimeToTicks(ent->GetSimulationTime() - ent->GetOldSimulationTime()));

	// if shit tick delta
	if (tick_delta > 15 && tick_delta < 2)
		return;

	// get the next position
	Vector nextPos = ExtrapolateTick(ent->GetVecOrigin(), ent->GetVecVelocity(), tick_delta);

	// set the next position 
	ent->SetAbsOrigin(nextPos);
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
			return;

		// Update based on info
		Update(ent, &info);

		// restore the origins
		Restore(ent);
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
