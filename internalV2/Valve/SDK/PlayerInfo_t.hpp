#pragma once
struct PlayerInfo_t
{
	std::uint64_t	ullVersion = 0ULL;
	union
	{
		std::uint64_t ullXuid;
		struct
		{
			std::uint32_t nXuidLow;
			std::uint32_t nXuidHigh;
		};
	};

	char			szName[128];
	int				nUserID;
	char			szSteamID[33];
	std::uint32_t	nFriendsID;
	char			szFriendsName[128];
	bool			bFakePlayer;
	bool			bIsHLTV;
	unsigned int customfiles[4];
	std::uint8_t	dFilesDownloaded;
};