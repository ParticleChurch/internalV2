#pragma once
class IEngineClient
{
public:
	void GetScreenSize(int& iWidth, int& iHeight)
	{
		CallVFunc<void>(this, 5, std::ref(iWidth), std::ref(iHeight));
	}

	bool GetPlayerInfo(int nEntityIndex, PlayerInfo_t* pInfo)
	{
		return CallVFunc<bool>(this, 8, nEntityIndex, pInfo);
	}

	int GetPlayerForUserID(int nUserID)
	{
		return CallVFunc<int>(this, 9, nUserID);
	}

	bool IsConsoleVisible()
	{
		return CallVFunc<bool>(this, 11);
	}

	int GetLocalPlayer()
	{
		return CallVFunc<int>(this, 12);
	}

	float GetLastTimeStamp()
	{
		return CallVFunc<float>(this, 14);
	}

	void GetViewAngles(QAngle& angView)
	{
		CallVFunc<void>(this, 18, std::ref(angView));
	}

	void SetViewAngles(QAngle& angView)
	{
		CallVFunc<void>(this, 19, std::ref(angView));
	}

	int GetMaxClients()
	{
		return CallVFunc<int>(this, 20);
	}

	// returns true if the player is fully connected and active in game (i.e, not still loading) and for check doesnt need isconnected
	bool IsInGame()
	{
		return CallVFunc<bool>(this, 26);
	}

	// returns true if the player is connected, but not necessarily active in game (could still be loading)
	bool IsConnected()
	{
		return CallVFunc<bool>(this, 27);
	}

	bool IsDrawingLoadingImage()
	{
		return CallVFunc<bool>(this, 28);
	}

	const ViewMatrix_t& WorldToScreenMatrix()
	{
		return CallVFunc<const ViewMatrix_t&>(this, 37);
	}

	void* GetBSPTreeQuery()
	{
		return CallVFunc<void*>(this, 43);
	}

	const char* GetLevelName()
	{
		return CallVFunc<const char*>(this, 52);
	}

	const char* GetLevelNameShort()
	{
		return CallVFunc<const char*>(this, 53);
	}

	INetChannelInfo* GetNetChannelInfo()
	{
		return CallVFunc<INetChannelInfo*>(this, 78);
	}

	bool IsPlayingDemo()
	{
		return CallVFunc<bool>(this, 82);
	}

	bool IsRecordingDemo()
	{
		return CallVFunc<bool>(this, 83);
	}

	bool IsPlayingTimeDemo()
	{
		return CallVFunc<bool>(this, 84);
	}

	bool IsTakingScreenshot()
	{
		return CallVFunc<bool>(this, 92);
	}

	bool IsHLTV()
	{
		return CallVFunc<bool>(this, 93);
	}

	unsigned int GetEngineBuildNumber()
	{
		return CallVFunc<unsigned int>(this, 104);
	}

	const char* GetProductVersionString()
	{
		return CallVFunc<const char*>(this, 105);
	}

	void ExecuteClientCmd(const char* szCmdString)
	{
		CallVFunc<void>(this, 108, szCmdString);
	}

	void ClientCmdUnrestricted(const char* szCmdString, bool bFromConsoleOrKeybind = false)
	{
		CallVFunc<void>(this, 114, szCmdString, bFromConsoleOrKeybind);
	}

	/*SteamAPIContext_t* GetSteamAPIContext()
	{
		return CallVFunc<SteamAPIContext_t*>(this, 185);
	}*/

	bool IsVoiceRecording()
	{
		return CallVFunc<bool>(this, 224);
	}
};