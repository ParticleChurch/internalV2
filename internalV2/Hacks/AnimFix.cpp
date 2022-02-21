#include "../Include.hpp"

AnimFix* animfix = new AnimFix();

#define DEBUG_ANIMFIX true

void anim_data_t::backup(Entity* player)
{
	if constexpr (DEBUG_ANIMFIX) L::Debug("backup");
	o_flags = player->GetFlags();
	o_duck_amount = player->GetDuckAmount();
}

void anim_data_t::restore(Entity* player)
{
	if constexpr (DEBUG_ANIMFIX) L::Debug("restore");

	player->GetFlags() = o_flags;
	player->GetDuckAmount() = o_duck_amount;
}

void AnimFix::lag_player(Entity* ent, int i)
{
	if (i >= 64) return;

	static Vector lastPos[64];
	static float lastTime[64];
	static Vector backupAbsPos[64];
	static Vector backupPos[64];
	static float backupTime[64];

	if constexpr (DEBUG_ANIMFIX) L::Debug("lag_player");

	backupPos[i] = ent->GetVecOrigin();
	backupAbsPos[i] = ent->GetAbsOrigin();
	backupTime[i] = ent->GetOldSimulationTime();

	// 0 lag rn
	if (lag == 1)
	{
		lastTime[i] = ent->GetSimulationTime();
		lastPos[i] = ent->GetVecOrigin();
	}
	// lets STRESS TEST the fuckin SYSTEM BABY
	/*else
	{*/
		ent->GetVecOrigin() = lastPos[i];
		ent->SetAbsOrigin(lastPos[i]);
		ent->GetOldSimulationTime() = lastTime[i];
	//}
}

void AnimFix::extrapolate_player(Entity* ent, int i)
{
	static Vector ValidPos[64];
	static Vector ValidVel[64];
	static float ValidSimtime[64];
	static int LastLag[64];

	int lagAmount = abs(TimeToTicks(ent->GetSimulationTime() - ent->GetOldSimulationTime()));

	// if dummy thic lagg amount
	if (lagAmount > 4 || lagAmount < 0)
		return;

	if (lagAmount == 0)
	{
		ValidVel[i] = (ent->GetVecOrigin() - ValidPos[i]) / ((LastLag[i] + 1) * I::globalvars->flIntervalPerTick);
		ValidPos[i] = ent->GetVecOrigin();
		ValidSimtime[i] = ent->GetSimulationTime();
		return;
	}
	LastLag[i] = lagAmount;

	Vector newPos = Calc::ExtrapolateTick(ValidPos[i], ValidVel[i], lagAmount - 1);
	newPos.z = ValidPos[i].z; // otherwise big dumb happens and im lazy
	//ent->GetVecVelocity() = ValidVel[i];
	if (newPos != Vector(0,0,0) &&  (newPos - ValidPos[i]).VecLength2D() < 2048)
	{
		ent->SetAbsOrigin(newPos);
		ent->GetVecOrigin() = newPos;
	}
	
	ent->GetSimulationTime() = ValidSimtime[i] + TicksToTime(lagAmount);

}

// quack fix
void AnimFix::duck_fix(Entity* player)
{
	if constexpr (DEBUG_ANIMFIX) L::Debug("duck_fix");

	int index = player->GetIndex();

	anim_data_t* anim_data = &m_data[index];
	PlayerInfo_t info;
	if (!I::engine->GetPlayerInfo(index, &info))
		return;
	int UserID = info.nUserID;

	LagComp::Player* player_data = &lagcomp->PlayerList[UserID];

	if (!player_data->Dormant && player_data->LifeState == LIFE_ALIVE && player_data->Health > 0)
	{
		float old_duck = anim_data->duck_amount;
		anim_data->duck_amount = player->GetDuckAmount();

		int ticks = TimeToTicks(player->GetSimulationTime() - player->GetOldSimulationTime());

		float duck_change = anim_data->duck_amount - old_duck;
		float duck_1tick_change = duck_change / ticks;

		player->GetDuckAmount() = old_duck + duck_1tick_change;
	}
	else
	{
		anim_data->duck_amount = player->GetDuckAmount();
	}
}

void AnimFix::jump_fix(Entity* player)
{
	if constexpr (DEBUG_ANIMFIX) L::Debug("jump_fix");

	int index = player->GetIndex();

	anim_data_t* anim_data = &m_data[index];
	PlayerInfo_t info;
	if (!I::engine->GetPlayerInfo(index, &info))
		return;
	int UserID = info.nUserID;

	LagComp::Player* player_data = &lagcomp->PlayerList[UserID];

	if (!player_data->Dormant && player_data->LifeState == LIFE_ALIVE && player_data->Health > 0)
	{
		int previous_flags = anim_data->flags;
		anim_data->flags = player->GetFlags();

		player->GetFlags() = previous_flags;
		player->GetFlags() &= ~FL_ONGROUND;

		if (anim_data->flags & FL_ONGROUND && previous_flags & FL_ONGROUND)
		{
			player->GetFlags() |= FL_ONGROUND;
		}
		else
		{
			if (player_data->AnimLayers[4].flWeight != 1.f && player_data->OldAnimLayers[4].flWeight == 1.f && player_data->AnimLayers[5].flWeight != 0.f)
			{
				player->GetFlags() |= FL_ONGROUND;
			}


			if (anim_data->flags & FL_ONGROUND && !(previous_flags & FL_ONGROUND))
			{
				player->GetFlags() &= ~FL_ONGROUND;
			}
		}
	}
	else
	{
		anim_data->flags = player->GetFlags();
	}
}

void AnimFix::pre_anim_update(Entity* player)
{
	if constexpr (DEBUG_ANIMFIX) L::Debug("pre_anim_update");

	int index = player->GetIndex();

	anim_data_t* anim_data = &m_data[index];
	PlayerInfo_t info;
	if (!I::engine->GetPlayerInfo(index, &info))
		return;
	int UserID = info.nUserID;

	LagComp::Player* player_data = &lagcomp->PlayerList[UserID];

	// restore anim layers to raw state
	memcpy(player->GetAnimOverlays(), player_data->AnimLayers, 15 * sizeof CAnimationLayer);
}

void AnimFix::anim_update(Entity* player)
{
	if constexpr (DEBUG_ANIMFIX) L::Debug("anim_update");

	int index = player->GetIndex();

	if (index >= 65) return;

	anim_data_t* anim_data = &m_data[index];

	// backup original data
	float o_curtime = I::globalvars->flCurrentTime;
	float o_frametime = I::globalvars->flFrameTime;
	int   o_framecount = I::globalvars->iFrameCount;
	int   o_tickcount = I::globalvars->iTickCount;

	// simulate animations at anim update time from server
	float anim_time = player->GetOldSimulationTime() + I::globalvars->flIntervalPerTick;
	int   anim_ticks = TimeToTicks(anim_time);

	I::globalvars->flCurrentTime = anim_time;
	I::globalvars->flFrameTime = I::globalvars->flIntervalPerTick;
	I::globalvars->iFrameCount = anim_ticks;
	I::globalvars->iTickCount = anim_ticks;

	// allow anim updates
	if constexpr (DEBUG_ANIMFIX) L::Debug("*player->ClientAnimations() true");
	*player->ClientAnimations() = true;

	// update anims
	if constexpr (DEBUG_ANIMFIX) L::Debug("UpdateClientSideAnimation");
	player->UpdateClientSideAnimation();

	// stop game from updating anims
	if constexpr (DEBUG_ANIMFIX) L::Debug("*player->ClientAnimations() false");
	//*player->ClientAnimations() = G::KillDLL; //let go of anims when we are unloading cheat (SET TO TRUE AFTER)

											  // restore original data
	I::globalvars->flCurrentTime = o_curtime;
	I::globalvars->flFrameTime = o_frametime;
	I::globalvars->iFrameCount = o_framecount;
	I::globalvars->iTickCount = o_tickcount;

	player->SetAbsOrigin(player->GetVecOrigin());
	//player->InvalidateBoneCache();
	/*static matrix3x4_t temp_matrix[128];
	player->SetupBones(temp_matrix, 128, BONE_USED_BY_ANYTHING, player->GetSimulationTime());*/
}

void AnimFix::post_anim_update(Entity* player)
{
	if constexpr (DEBUG_ANIMFIX) L::Debug("post_anim_update");

	int index = player->GetIndex();

	anim_data_t* anim_data = &m_data[index];
	PlayerInfo_t info;
	if (!I::engine->GetPlayerInfo(index, &info))
		return;
	int UserID = info.nUserID;

	LagComp::Player* player_data = &lagcomp->PlayerList[UserID];

	// restore anim layers to raw state
	memcpy(player->GetAnimOverlays(), player_data->AnimLayers, 15 * sizeof CAnimationLayer);
}

void AnimFix::Update(int stage)
{
	return;

	if constexpr (DEBUG_ANIMFIX) L::Debug("Update");

	if (stage != FRAME_NET_UPDATE_POSTDATAUPDATE_START) return;

	static int LastTickCount = I::globalvars->iTickCount;
	if (LastTickCount != I::globalvars->iTickCount)
	{
		LastTickCount = I::globalvars->iTickCount;
		lag++;
	}
	if (lag > 20)
		lag = 1;

	for (int i = 1; i < 65; i++)
	{
		Entity* ent = I::entitylist->GetClientEntity(i);
		if (!ent)
			continue;

		if (!(ent->GetHealth() > 0)) continue;

		if (!ent->IsPlayer()) continue;

		if (ent->IsDormant() || !ent->IsAlive()) continue;

		// local player requires different method of calculating animations
		// this is because with networked entities, we wait for update
		// the data in this update is based on anim update from animtime (old simtime + 1)

		// on local player however, we already got data to simulate right when we started choking, we dont have to wait for send and then simulate back in time
		if (ent == G::Localplayer)
			continue;

		anim_data_t* anim_data = &m_data[i];

		//lag_player(ent, i);

		//extrapolate_player(ent, i);

		if (ent->GetSimulationTime() != ent->GetOldSimulationTime())
		{
			anim_data->backup(ent);

			origin_fix(ent);
			velocity_fix(ent);
			jump_fix(ent);
			duck_fix(ent);

			pre_anim_update(ent);
			anim_update(ent);
			post_anim_update(ent);

			anim_data->restore(ent);
		}
	}
}

void AnimFix::origin_fix(Entity* player)
{
	if constexpr (DEBUG_ANIMFIX) L::Debug("origin_fix");

	// s/o to senator for 1 line interpolation fix
	player->SetAbsOrigin(player->GetVecOrigin());
}

void AnimFix::velocity_fix(Entity* player)
{
	if constexpr (DEBUG_ANIMFIX) L::Debug("velocity_fix");

	return;

	// more has to be done here
	int index = player->GetIndex();

	anim_data_t* anim_data = &m_data[index];
	PlayerInfo_t info;
	if (!I::engine->GetPlayerInfo(index, &info))
		return;
	int UserID = info.nUserID;

	LagComp::Player* player_data = &lagcomp->PlayerList[UserID];

	// for now assume that there is no fakewalking in 2022
	/*if (player_data->is_fakewalking)
	{
		player->velocity() = vec3();
	}*/
}
