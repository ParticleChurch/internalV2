#include "../Include.hpp"

EnginePrediction* enginePrediction = new EnginePrediction();

#define sig_prediction_random_seed "8B 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 83 C4 04"
void EnginePrediction::start(CUserCmd* cmd)
{
	if (!G::Localplayer)
		return;

	if (!prediction_random_seed)
		prediction_random_seed = *reinterpret_cast<int**>(FindPattern("client.dll", sig_prediction_random_seed) + 2);

	*prediction_random_seed = cmd->iRandomSeed & 0x7FFFFFFF;

	old_cur_time = I::globalvars->flCurrentTime;
	old_frame_time = I::globalvars->flFrameTime;

	I::globalvars->flCurrentTime = G::Localplayer->GetTickBase() * I::globalvars->flIntervalPerTick;
	I::globalvars->flFrameTime = I::globalvars->flIntervalPerTick;

	I::gamemovement->start_track_prediction_errors(G::Localplayer);

	memset(&data, 0, sizeof(data));
	I::playermovehelper->set_host(G::Localplayer);
	I::prediction->setup_move(G::Localplayer, cmd, I::playermovehelper, &data);
	I::gamemovement->process_movement(G::Localplayer, &data);
	I::prediction->finish_move(G::Localplayer, cmd, &data);
}

void EnginePrediction::end()
{
	if (!G::Localplayer)
		return;

	I::gamemovement->finish_track_prediction_errors(G::Localplayer);
	I::playermovehelper->set_host(nullptr);

	*prediction_random_seed = -1;

	I::globalvars->flCurrentTime = old_cur_time;
	I::globalvars->flFrameTime = old_frame_time;
}
