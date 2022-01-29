#include "../Include.hpp"

AnimFix* animfix = new AnimFix();

#define DEBUG_ANIMFIX false

void anim_data_t::backup(Entity* player)
{
	o_flags = player->GetFlags();
	o_duck_amount = player->GetDuckAmount();
}

void anim_data_t::restore(Entity* player)
{
	player->GetFlags() = o_flags;
	player->GetDuckAmount() = o_duck_amount;
}

// quack fix
void AnimFix::duck_fix(Entity* player)
{
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
	int index = player->GetIndex();

	anim_data_t* anim_data = &m_data[index];
	PlayerInfo_t info;
	if (!I::engine->GetPlayerInfo(index, &info))
		return;
	int UserID = info.nUserID;

	LagComp::Player* player_data = &lagcomp->PlayerList[UserID];

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
	*player->ClientAnimations() = true;

	// update anims
	player->UpdateClientSideAnimation();

	// stop game from updating anims
	*player->ClientAnimations() = false; //let go of anims when we are unloading cheat (SET TO TRUE AFTER)

											  // restore original data
	I::globalvars->flCurrentTime = o_curtime;
	I::globalvars->flFrameTime = o_frametime;
	I::globalvars->iFrameCount = o_framecount;
	I::globalvars->iTickCount = o_tickcount;

	/*player->SetAbsOrigin(player->GetVecOrigin());*/
	//player->InvalidateBoneCache();
	/*static matrix3x4_t temp_matrix[128];
	player->SetupBones(temp_matrix, 128, BONE_USED_BY_ANYTHING, player->GetSimulationTime());*/
}

void AnimFix::post_anim_update(Entity* player)
{
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
	if (stage != FRAME_NET_UPDATE_POSTDATAUPDATE_START) return;

	for (int i = 1; i < 65; i++)
	{
		Entity* ent = I::entitylist->GetClientEntity(i);
		if (!ent)
			continue;

		if (ent->IsDormant() || !ent->IsAlive())
			continue;

		// local player requires different method of calculating animations
		// this is because with networked entities, we wait for update
		// the data in this update is based on anim update from animtime (old simtime + 1)

		// on local player however, we already got data to simulate right when we started choking, we dont have to wait for send and then simulate back in time
		if (ent == G::Localplayer)
			continue;

		anim_data_t* anim_data = &m_data[i];

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
	// s/o to senator for 1 line interpolation fix
	player->SetAbsOrigin(player->GetVecOrigin());
}

void AnimFix::velocity_fix(Entity* player)
{
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
