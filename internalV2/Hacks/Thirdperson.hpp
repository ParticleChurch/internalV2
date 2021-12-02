#pragma once
class Thirdperson
{
private:
	Vector GetIdealCameraPos(float distance);
	float GetCameraBoomLength(float distance);
public:
	void Run_hkCamToFirstPeron();
	void Run_DoPostScreenEffects();
};

extern Thirdperson* thirdperson;

