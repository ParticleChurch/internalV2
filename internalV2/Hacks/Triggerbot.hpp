#pragma once
class Triggerbot
{
private:
	std::deque< EHitboxIndex> Hitboxes;
	float CalculatePsudoHitchance();
	void GetHitboxes();
public:
	void Run();
};

extern Triggerbot* triggerbot;
