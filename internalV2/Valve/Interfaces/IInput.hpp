#pragma once
#define MULTIPLAYER_BACKUP 150

class IInput
{
public:
	char _pad0[12];
	bool isTrackIRAvailable;
	bool isMouseInitialized;
	bool isMouseActive;
	char _pad1[0x9E];
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

	/*CUserCmd* GetUserCmd(int nSequenceNumber)
	{
		return &commands[nSequenceNumber % MULTIPLAYER_BACKUP];
	}*/

	CUserCmd* GetUserCmd(int sequence_number)
	{
		using OriginalFn = CUserCmd * (__thiscall*)(void*, int, int);
		return GetVFunc<OriginalFn>(this, 8)(this, 0, sequence_number);
	}

	CUserCmd* GetUserCmd(int nSlot, int sequence_number)
	{
		typedef CUserCmd* (__thiscall* GetUserCmd_t)(void*, int, int);
		return GetVFunc<GetUserCmd_t>(this, 8)(this, nSlot, sequence_number);
	}

	CVerifiedUserCmd* GetVerifiedCmd(int nSequenceNumber)
	{
		return &verified_commands[nSequenceNumber % MULTIPLAYER_BACKUP];
	}
};
