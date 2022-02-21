#include "../Include.hpp"

Resolver* resolver = new Resolver();

void Resolver::AimbotShot(aim_shot_log shot)
{
	/*float latency = (I::engine->GetNetChannelInfo()->GetAvgLatency(0) + I::engine->GetNetChannelInfo()->GetAvgLatency(1));
	shot.time -= latency;*/
	
	
	//H::console.push_back("AIM: " + std::to_string(shot.time));
	

	aim_shots.push_back(shot);

}

void Resolver::Shot()
{
	static int last_shot = 0;

	if (last_shot == I::globalvars->iTickCount )
	{
		//we can only shoot once per tick :>
		//because it was called twice in 1 cm move tick, this means that a shot was fired by aimbot, and at end of cm move, it registered again
		//we need to register at end of cm tho, so manual shooting doesnt fuck shit up
		return;
	}

	shot_log new_shot;
	new_shot.time = I::globalvars->flRealTime;
	new_shot.flCurrentTime = I::globalvars->flCurrentTime;
	shots.push_back(new_shot);

	last_shot = I::globalvars->iTickCount;

	// DO FIX
	if (aim_shots.empty())
		return;
	float latency = (I::engine->GetNetChannelInfo()->GetAvgLatency(0) + I::engine->GetNetChannelInfo()->GetAvgLatency(1)) * 2.f;
	float maxtime = 0.1f + latency;
	if (fabsf(new_shot.flCurrentTime - aim_shots.back().time) < maxtime)
	{
		aim_shots.back().time = new_shot.time;
		aim_shots.back().fixed = true;
	}
}

void Resolver::Impact(Vector pos)
{
	impact_log new_impact;
	new_impact.end_pos = pos;
	new_impact.time = I::globalvars->flRealTime;
	new_impact.impacts.push_back(pos);

	impacts.push_back(new_impact);
}

void Resolver::Hurt(int userID, int hitgroup, int damage)
{
	hurts.push_back({ I::globalvars->flRealTime, userID, hitgroup, damage });
}

void Resolver::SortImpacts()
{
	Vector origin = G::Localplayer->GetVecOrigin();

	std::vector<impact_log> new_impacts;

	for (size_t i = 0; i < this->impacts.size(); i++)
	{
		bool new_time = true;
		int stored_impact = 0;

		for (size_t j = 0; j < new_impacts.size(); j++)
		{
			if (new_impacts[j].time == this->impacts[i].time)
			{
				new_time = false;
				stored_impact = j;
				break;
			}
		}

		if (new_time)
		{
			new_impacts.push_back(this->impacts[i]);
		}
		else
		{
			float stored_dist = origin.Dist(new_impacts[stored_impact].end_pos);
			float new_dist = origin.Dist(this->impacts[i].end_pos);

			if (new_dist > stored_dist)
				new_impacts[stored_impact].end_pos = this->impacts[i].end_pos;

			new_impacts[stored_impact].impacts.push_back(this->impacts[i].end_pos);
		}
	}

	this->impacts.clear();
	for (size_t i = 0; i < new_impacts.size(); i++)
	{
		this->impacts.push_back(new_impacts[i]);
	}
}

void Resolver::MatchImpacts()
{
	// cleanup old records
	float latency = (I::engine->GetNetChannelInfo()->GetAvgLatency(0) + I::engine->GetNetChannelInfo()->GetAvgLatency(1)) * 2.f;
	float maxtime = 0.1f + latency;

	float realtime = I::globalvars->flRealTime;
	for (size_t i = 0; i < this->shots.size(); i++)
	{
		if (std::abs(realtime - this->shots.at(i).time) > maxtime)
		{
			this->shots.erase(this->shots.begin() + i);
			i--;
		}
	}

	if (this->impacts.empty())
		return;

	// match data
	std::vector<match_log> new_matches;

	for (size_t i = 0; i < this->impacts.size(); i++)
	{
		if (this->shots.empty())
			break;

		impact_log* impact = &this->impacts[i];
		shot_log shot = this->shots.front();
		match_log new_match;
		new_match.miss = true;

		//match it to hurt
		for (size_t j = 0; j < this->hurts.size(); j++)
		{
			if (impact->time == this->hurts[j].time)
			{
				new_match.hurt = *&this->hurts[j];
				new_match.miss = false;
				break;
			}
		}
		
		new_match.shot = shot;
		new_match.impact = *impact;

		new_matches.push_back(new_match);
		this->shots.pop_front();
	}

	this->impacts.clear();
	this->hurts.clear();
	this->matches.clear();

	for (size_t i = 0; i < new_matches.size(); i++)
	{
		//H::console.push_back("MATCH: " + std::to_string(new_matches[i].impact.time));
		this->matches.push_back(new_matches[i]);
	}
		
}

void Resolver::ResolveMatches()
{
	if (this->matches.empty()) return;

	/*H::console.clear();
	H::console.resize(0);*/

	while (H::console.size() > 10)
		H::console.erase(H::console.begin());

	// should prob iterate through aimshots, and determine if any matches match, but whatev lol
	for (size_t i = 0; i < this->matches.size(); i++)
	{
		match_log& match = this->matches[i];

		/*H::console.push_back("match time: " + std::to_string(match.impact.time));*/

		int AimbotShotIndex = -1;
		for (unsigned int j = 0; j < this->aim_shots.size(); j++)
		{
			/*H::console.push_back("--aimbot time: " + std::to_string(aim_shots[i].time) + (aim_shots[i].fixed ? " (fixed)" : " (broken)"));
			H::console.push_back("--	DELTA: " + std::to_string(fabsf(match.shot.time - this->aim_shots[j].time)) + " VS " + std::to_string(I::globalvars->flIntervalPerTick + 0.1f));*/

			if (fabsf(match.shot.time - this->aim_shots[j].time) < I::globalvars->flIntervalPerTick + 0.1f)
				AimbotShotIndex = j;
		}

		// prob just a normal shot, ignore
		if (AimbotShotIndex == -1) continue;

		aim_shot_log aim = this->aim_shots[AimbotShotIndex];

		std::string matchName = "";
		PlayerInfo_t info;
		I::engine->GetPlayerInfo(I::engine->GetPlayerForUserID(aim.userID), &info);

		std::string otherinfo = " ";
		otherinfo += "DAM[" + std::to_string(aim.damage) + "/" + std::to_string(match.hurt.damage) + "]";
		otherinfo += " HC[" + std::to_string(aim.hitchance) + "]";
		otherinfo += " SP[" + std::to_string(aim.safepoint) + "]";

		//:D good, :P bad, :/ spread, D: trace error

		// we actually hit something
		bool did_hit = false;
		if (!match.miss)
		{
			// right higroup, right person...
			if (match.hurt.hitgroup == aim.hitgroup && match.hurt.userid == aim.userID)
			{
				lagcomp->PlayerList[match.hurt.userid].goodShots++;
				H::console.push_back(":D on " + (std::string)info.szName + otherinfo);
				this->aim_shots.erase(this->aim_shots.begin() + AimbotShotIndex);
				continue;
			} 
			else if (match.hurt.userid == aim.userID)
			{
				// lucky spread hit
				lagcomp->PlayerList[match.hurt.userid].luckShots++;
				H::console.push_back("hit shot do to spread on " + (std::string)info.szName + otherinfo);
				this->aim_shots.erase(this->aim_shots.begin() + AimbotShotIndex);
				continue;
			}
			// otherwise its gonna be a spread/resolve issue
			did_hit = true;
		}

		bool IntersectsHitbox = DoesRayIntersectHitbox(aim.min_bone, aim.max_bone, aim.radius_bone, aim.shootPos, match.impact.end_pos);
		if (IntersectsHitbox)
		{
			// resolve error
			lagcomp->PlayerList[aim.userID].badShots++;
			H::console.push_back(":P on " + (std::string)info.szName + otherinfo);
			this->aim_shots.erase(this->aim_shots.begin() + AimbotShotIndex);
			continue;
		}

		if ((aim.shootPos - aim.shootPoint).VecLength() > (aim.shootPos - match.impact.end_pos).VecLength() + 5.f && aim.hitchance > 50.f)
		{
			// ray didn't reach far enough --> autowall error, not resolver error
			H::console.push_back("D: on " + (std::string)info.szName + otherinfo);
			this->aim_shots.erase(this->aim_shots.begin() + AimbotShotIndex);
			continue;
		}

		// spread error
		H::console.push_back(":/ on " + (std::string)info.szName + otherinfo);
		this->aim_shots.erase(this->aim_shots.begin() + AimbotShotIndex);
	}

	/*this->aim_shots.clear();
	this->aim_shots.resize(0);*/
}

void Resolver::LogicResolve(Entity* ent, int missedShots)
{		
	auto animstate = ent->GetAnimState();
	if (!animstate) return;

	// in air fix
	bool onGround = ent->GetFlags() & FL_ONGROUND;
	if (!onGround)
	{
		animstate->flGoalFeetYaw = ent->GetLBY();
		return;
	}
		
	Vector velocity = ent->GetVecVelocity();
	float horizontalVelocity = velocity.VecLength2D();
	float maxSpeed = ent->MaxAccurateSpeed() / 3.f;
	float maxDesync = ent->GetMaxDesyncDelta(animstate);
	
	float backAngle = Calc::CalculateAngle(G::Localplayer->GetVecOrigin(), ent->GetVecOrigin()).y;

	float eye_yaw = animstate->flEyeYaw;
	float value = eye_yaw;

	// slowwalk check --> actually do proper bruteforce
	bool slowWalking = horizontalVelocity >= 1.f && horizontalVelocity <= maxSpeed;
	
	switch (missedShots % 6)
	{
	case 0:
		value = ent->GetLBY();
		break;
	case 1:
		value = eye_yaw + maxDesync;
		break;
	case 2:
		value = eye_yaw - maxDesync;
		break;
	case 3:
		value = eye_yaw + maxDesync / 3;
		break;
	case 4:
		value = eye_yaw - maxDesync / 3;
		break;
	case 5:
		value = backAngle;
		break;
	}


	/*value = eye_yaw + 10.f * missedShots;*/
	/*int side = missedShots % 2 == 0;
	value = eye_yaw + ((rand() % int(maxDesync)) / 2 * side);*/

	animstate->flGoalFeetYaw = NormalizeYaw(value);
}

void Resolver::Run()
{ 
	L::Debug("RESOLVER");
	// sort all the events before making a decision

	SortImpacts();
	MatchImpacts();
	ResolveMatches();

	//return; // lets be extra careful here now

	static int retard = 0;
	static int lastretard = G::Localplayer->GetAmmo();
	if (lastretard != G::Localplayer->GetAmmo())
	{
		lastretard = G::Localplayer->GetAmmo();
		retard++;
	}
	
	// make decision
	for (auto a : lagcomp->PlayerList)
	{
		auto& player = a.second;
		
		if (!player.pEntity) return;
		if (player.Dormant) return;
		if (!(player.Health > 0)) return;
		if (player.Team == G::LocalplayerTeam) return;
		if (player.LifeState != LIFE_ALIVE) return;

		Entity* ent = I::entitylist->GetClientEntity(player.Index);
		if (!ent) continue;
		if (!(ent->GetHealth() > 0)) continue;
		if (ent->GetLifeState() != LIFE_ALIVE) continue;
		if (ent->GetTeam() == G::LocalplayerTeam) continue;
		if (ent->IsDormant()) continue;


		LogicResolve(ent, player.badShots);
	}

}


