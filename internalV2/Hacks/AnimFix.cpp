#include "../Include.hpp"

AnimFix* animfix = new AnimFix();

#define DEBUG_ANIMFIX false

void AnimFix::UpdateEntAnim(Entity* ent)
{
}

void AnimFix::RunAnimFix(int stage)
{
	//FRAME_NET_UPDATE_POSTDATAUPDATE_START FRAME_RENDER_START
	if (stage != FRAME_NET_UPDATE_END) return;

	// don't bother updating if not valid localplayer
	if (!G::Localplayer) return;
	if (!G::LocalplayerAlive) return;

	if constexpr (DEBUG_ANIMFIX) L::Debug("FrameStageNotifyHook animfix", "\n");

	Entity* e;
	for (int i = 1; i < I::engine->GetMaxClients(); ++i)
	{
		// Localplayer Check
		if (i == G::LocalplayerIndex) continue;

		// entity existence check
		e = I::entitylist->GetClientEntity(i);
		if (!e) continue;

		// entity player check
		if (!e->IsPlayer()) continue;

		// just do enemies for now
		if (e->GetTeam() == G::LocalplayerTeam) continue;

		// do another player check
		PlayerInfo_t info;
		if (!I::engine->GetPlayerInfo(i, &info))
			return;

		// run actual animfix
		auto time_delta = abs(TimeToTicks(e->GetSimulationTime()) - I::globalvars->iTickCount);

		// if disgusting sim time...
		if (time_delta > 1.0f / I::globalvars->flIntervalPerTick)
			continue;

		auto update = player_records[i].empty() || e->m_flSimulationTime() != e->m_flOldSimulationTime(); //-V550
	}
}
