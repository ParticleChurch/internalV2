#pragma once
#define MULTIPLAYER_BACKUP 150

class IInput
{
public:
	char _pad0[12];
	bool isTrackIRAvailable;
	bool isMouseInitialized;
	bool isMouseActive;
	char _pad1[178];
	bool m_fCameraInThirdPerson;
	bool m_fCameraMovingWithMouse;
	Vector m_vecCameraOffset;
	bool m_fCameraDistanceMove;
	int m_nCameraOldX;
	int m_nCameraOldY;
	int m_nCameraX;
	int m_nCameraY;
	bool m_CameraIsOrthographic;
	Vector m_angPreviousViewAngles;
	Vector m_angPreviousViewAnglesTilt;
	float m_flLastForwardMove{};
	int m_nClearInputState{};
	CUserCmd* commands{};
	CVerifiedUserCmd* verified_commands{};

	CUserCmd* GetUserCmd(int nSequenceNumber)
	{
		return &commands[nSequenceNumber % MULTIPLAYER_BACKUP];
	}

	CVerifiedUserCmd* GetVerifiedCmd(int nSequenceNumber)
	{
		return &verified_commands[nSequenceNumber % MULTIPLAYER_BACKUP];
	}
};
