#pragma once
class EnginePrediction
{
public:
	void start(CUserCmd* cmd);
	void end();

private:
	player_move_data data;
	float old_cur_time;
	float old_frame_time;
	int* prediction_random_seed;
};

extern EnginePrediction* enginePrediction;

