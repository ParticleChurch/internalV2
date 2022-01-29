#pragma once
struct anim_data_t
{
	int flags{ };
	float duck_amount{ };

	int o_flags{ };
	float o_duck_amount{ };

	void backup(Entity* player);
	void restore(Entity* player);
};

// ONLY FOR ENEMYS FOR NOW
class AnimFix
{
private:
	anim_data_t m_data[65]{ };
	void duck_fix(Entity* player);
	void jump_fix(Entity* player);
	void origin_fix(Entity* player);
	void velocity_fix(Entity* player);

	void pre_anim_update(Entity* player);
	void anim_update(Entity* player);
	void post_anim_update(Entity* player);
public:
	void Update(int stage);
};

extern AnimFix* animfix;

