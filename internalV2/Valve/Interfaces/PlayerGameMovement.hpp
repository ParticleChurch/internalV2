class PlayerMoveHelper {
public:
	bool	first_run_of_iunctions : 1;
	bool	game_code_moved_player : 1;
	int	player_handle;
	int	impulse_command;
	Vector	view_angles;
	Vector	abs_view_angles;
	int	buttons;
	int	old_buttons;
	float	forward_move;
	float	side_move;
	float	up_move;
	float	max_speed;
	float	client_max_speed;
	Vector	velocity;
	Vector	angles;
	Vector	old_angles;
	float	out_step_height;
	Vector	wish_velocity;
	Vector	jump_velocity;
	Vector	constraint_center;
	float	constraint_radius;
	float	constraint_width;
	float	constraint_speed_factor;
	float	u0[5];
	Vector	abs_origin;
	virtual	void u1() = 0;
	virtual void set_host(Entity* host) = 0;
};

class player_move_data {
public:
	bool    first_run_of_instructions : 1;
	bool    game_code_moved_player : 1;
	int     player_handle;
	int     impulse_command;
	Vector	view_angles;
	Vector	abs_view_angles;
	int     buttons;
	int     old_buttons;
	float   fw_move;
	float   sd_move;
	float   up_move;
	float   max_speed;
	float   client_max_speed;
	Vector	velocity;
	Vector	angles;
	Vector	old_angles;
	float   step_height;
	Vector	wish_velocity;
	Vector	jump_velocity;
	Vector	constraint_center;
	float   constraint_radius;
	float   constraint_width;
	float   constraint_speed_factor;
	float   u0[5];
	Vector	abs_origin;
};

class virtual_game_movement {

public:
	virtual				~virtual_game_movement(void) {}
	virtual void			process_movement(Entity* player, player_move_data* move) = 0;
	virtual void			reset(void) = 0;
	virtual void			start_track_prediction_errors(Entity* player) = 0;
	virtual void			finish_track_prediction_errors(Entity* player) = 0;
	virtual void			diff_print(char const* fmt, ...) = 0;
	virtual Vector const& get_player_mins(bool ducked) const = 0;
	virtual Vector const& get_player_maxs(bool ducked) const = 0;
	virtual Vector const& get_player_view_offset(bool ducked) const = 0;
	virtual bool			is_moving_player_stuck(void) const = 0;
	virtual Entity* get_moving_player(void) const = 0;
	virtual void			unblock_posher(Entity* player, Entity* pusher) = 0;
	virtual void			setup_movement_bounds(player_move_data* move) = 0;
};

class PlayerGameMovement : public virtual_game_movement {
public:
	virtual ~PlayerGameMovement(void) { }
};

class PlayerPrediction {
public:
	bool in_prediction() {
		typedef bool(__thiscall* o_in_prediction)(void*);
		return GetVFunc<o_in_prediction>(this, 14)(this);
	}

	void run_command(Entity* player, CUserCmd* cmd, PlayerMoveHelper* helper) {
		typedef void(__thiscall* o_run_command)(void*, Entity*, CUserCmd*, PlayerMoveHelper*);
		return GetVFunc<o_run_command>(this, 19)(this, player, cmd, helper);
	}

	void setup_move(Entity* player, CUserCmd* cmd, PlayerMoveHelper* helper, void* data) {
		typedef void(__thiscall* o_setup_move)(void*, Entity*, CUserCmd*, PlayerMoveHelper*, void*);
		return GetVFunc<o_setup_move>(this, 20)(this, player, cmd, helper, data);
	}

	void finish_move(Entity* player, CUserCmd* cmd, void* data) {
		typedef void(__thiscall* o_finish_move)(void*, Entity*, CUserCmd*, void*);
		return GetVFunc<o_finish_move>(this, 21)(this, player, cmd, data);
	}
};