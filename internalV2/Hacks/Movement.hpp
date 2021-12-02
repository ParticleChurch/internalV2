#pragma once
class Movement
{
public:
	// Hacks
	void Bunnyhop();
	void AutoStrafe();
	// Other
	void CM_Clamp();
	void CM_Start(CUserCmd* cmd, bool* pSendPacket);
	void CM_MoveFixStart();
	void CM_MoveFixEnd();
	void CM_End();
};

extern Movement* movement;