#pragma once

class Resolver
{
public:
	struct shot_log
	{
		float time;
	};
	void Shot();
private:

	struct player_hurt_log
	{
		float time;
		int userid;
		int hitgroup;
		int damage;
	};
	struct impact_log
	{
		float time;
		Vector end_pos;
		std::vector<Vector> impacts;
	};
	struct match_log
	{
		bool miss;
		shot_log shot;
		impact_log impact;
		player_hurt_log hurt;

		std::string print()
		{
			std::string miss_str = "miss: " + miss ? "true" : "false";
			std::string shot_str = "shot: \n\t time: " + std::to_string(shot.time);
			std::string impact_str = "impact: \n\t time: " + std::to_string(impact.time);
			impact_str += "\n\t end_pos: " + impact.end_pos.str();
			for(auto i : impact.impacts)
				impact_str += "\n\t   -: " + i.str();
			std::string hurt_str = "hurt: \n\t time: " + std::to_string(hurt.time);
			hurt_str += "\n\t userid: " + std::to_string(hurt.userid);
			hurt_str += "\n\t hitgroup: " + std::to_string(hurt.hitgroup);
			hurt_str += "\n\t damage: " + std::to_string(hurt.damage);
			
			return miss_str + "\n" + shot_str + "\n" + impact_str + "\n" + hurt_str + "\n";
		}
	};

	std::deque<shot_log>		shots;
	std::deque<player_hurt_log> hurts;
	std::deque<impact_log>      impacts;
	std::deque<match_log>       matches;
public:
	void Impact(Vector pos);
	void Hurt(int userID, int hitgroup, int damage);
private:
	void SortImpacts();
	void MatchImpacts();
private:

	enum Resolver_States
	{
		Legit, // player is legit, dont resolve
		Logic, // this one will prob work the most
		Brute, // fuck it do stupid shit
		Random, // fuck it, do the stupid shit stupider
	};
	void LogicResolve(Entity* ent, int missedShots);
public:
	void Run();
};

extern Resolver* resolver;

