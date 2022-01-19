#include "../Include.hpp"

Resolver* resolver = new Resolver();

void Resolver::AimbotShot(aim_shot_log shot)
{
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
	shots.push_back(new_shot);

	last_shot = I::globalvars->iTickCount;
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
		player_hurt_log* hurt = nullptr;
		shot_log shot = this->shots.front();

		//match it to hurt
		for (size_t j = 0; j < this->hurts.size(); j++)
		{
			if (impact->time == this->hurts[j].time)
			{
				hurt = &this->hurts[j];
				break;
			}
		}

		match_log new_match;
		new_match.miss = !hurt;
		new_match.shot = shot;
		new_match.impact = *impact;
		new_match.hurt = hurt ? *hurt : player_hurt_log();

		new_matches.push_back(new_match);
		H::console.push_back(new_match.print());
		this->shots.pop_front();
	}

	this->impacts.clear();
	this->hurts.clear();
	this->matches.clear();

	for (size_t i = 0; i < new_matches.size(); i++)
		this->matches.push_back(new_matches[i]);
}

void Resolver::ResolveMatches()
{
	for (size_t i = 0; i < this->matches.size(); i++)
	{
		match_log& match = this->matches[i];

		int AimbotShotIndex = -1;
		for (int j = 0; j < this->aim_shots.size(); j++)
		{
			if (match.shot.time == this->aim_shots[j].time)
				AimbotShotIndex = j;
		}

		// prob just a normal shot, ignore
		if (AimbotShotIndex == -1) continue;

		aim_shot_log aim = this->aim_shots[AimbotShotIndex];

		H::console.push_back("____________________");

		// we actually hit something
		bool did_hit = false;
		if (!match.miss)
		{
			// right higroup, right person...
			if (match.hurt.hitgroup == aim.hitgroup && match.hurt.userid == aim.userID)
			{
				lagcomp->PlayerList[match.hurt.userid].goodShots++;
				H::console.push_back("good resolve");
				continue;
			} 
			else if (match.hurt.userid == aim.userID)
			{
				// lucky spread hit
				lagcomp->PlayerList[match.hurt.userid].luckShots++;
				H::console.push_back("lucky hit");
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
			H::console.push_back("bad resolve");
			continue;
		}

		if ((aim.shootPos - aim.shootPoint).VecLength() > (aim.shootPos - match.impact.end_pos).VecLength() + 5.f && aim.hitchance > 50.f)
		{
			// ray didn't reach far enough --> autowall error, not resolver error
			H::console.push_back("trace error");
			continue;
		}

		// spread error
		H::console.push_back("spread error");
	}

	/*this->aim_shots.clear();
	this->aim_shots.resize(0);*/

	
}

void Resolver::LogicResolve(Entity* ent, int missedShots)
{
	return;
	if (!ent) return;
	if (!(ent->GetHealth() > 0)) return;
	if (ent->GetLifeState() != LIFE_ALIVE) return;
	if (ent->IsDormant()) return;

	auto animstate = ent->GetAnimState();
	if (!animstate) return;

	// in air fix
	/*bool onGround = ent->GetFlags() & FL_ONGROUND;
	if (!onGround)
	{
		animstate->flGoalFeetYaw = ent->GetLBY();
		return;
	}*/
		
	Vector velocity = ent->GetVecVelocity();
	float horizontalVelocity = velocity.VecLength2D();
	float maxSpeed = ent->MaxAccurateSpeed() / 3.f;
	float maxDesync = ent->GetMaxDesyncDelta(animstate);
	

	float eye_yaw = animstate->flEyeYaw;
	float value = eye_yaw;

	// slowwalk check --> actually do proper bruteforce
	//bool slowWalking = horizontalVelocity >= 1.f && horizontalVelocity <= maxSpeed;
	switch (missedShots % 5)
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
	}

	animstate->flGoalFeetYaw = NormalizeYaw(value);
}

void Resolver::Run()
{ 
	return;
	// sort all the events before making a decision
	SortImpacts();
	MatchImpacts();
	ResolveMatches();
	
	// make decision
	for (auto a : lagcomp->PlayerList)
	{
		auto& player = a.second;
		
		if (!player.pEntity) return;
		if (player.Dormant) return;
		if (!(player.Health > 0)) return;
		if (player.Team == G::LocalplayerTeam) return;
		if (player.LifeState != LIFE_ALIVE) return;

		LogicResolve(player.pEntity, player.badShots);
	}

}
