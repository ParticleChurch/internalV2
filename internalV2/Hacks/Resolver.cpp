#include "../Include.hpp"

Resolver* resolver = new Resolver();

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
		this->shots.pop_front();
	}

	this->impacts.clear();
	this->hurts.clear();
	this->matches.clear();

	for (size_t i = 0; i < new_matches.size(); i++)
		this->matches.push_back(new_matches[i]);
}

void Resolver::LogicResolve(Entity* ent, int missedShots)
{
	CCSGOPlayerAnimState* animstate = ent->GetAnimState(ent);

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
	float maxDesync = ent->GetMaxDesyncDelta();

	// slowwalk check --> actually do proper bruteforce
	bool slowWalking = horizontalVelocity >= 1.f && horizontalVelocity <= maxSpeed;
	if (slowWalking)
	{
		switch (missedShots % 5)
		{
		case 0:
			animstate->flGoalFeetYaw = ent->GetLBY();
			break;
		case 1:
			animstate->flGoalFeetYaw += maxDesync;
			break;
		case 2:
			animstate->flGoalFeetYaw -= maxDesync;
			break;
		case 3:
			animstate->flGoalFeetYaw += maxDesync / 3;
			break;
		case 4:
			animstate->flGoalFeetYaw -= maxDesync / 3;
			break;
		}
		return;
	}
	else
	{
		switch (missedShots % 3)
		{
		case 0:
			animstate->flGoalFeetYaw = ent->GetLBY();
			break;
		case 1:
			animstate->flGoalFeetYaw += maxDesync;
			break;
		case 2:
			animstate->flGoalFeetYaw -= maxDesync;
			break;
		}
	}
}

void Resolver::Run()
{ 
	// sort all the events before making a decision
	SortImpacts();
	MatchImpacts();

	if (this->matches.size() > 0)
	{
		H::console.clear();
		H::console.resize(0);

		H::console.push_back("matches: ");
		for (auto a : this->matches)
		{
			H::console.push_back(a.print());
		}
	}
	
}
