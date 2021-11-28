#pragma once
struct Model_t
{
	std::byte	pad0[0x4];		//0x0000
	char		szName[260];	//0x0004
	int			nLoadFlags;		//0x0108
	int			nServerCount;	//0x010C
	int			iType;			//0x0110
	int			uFlags;			//0x0114
	Vector		vecMins;		//0x0118
	Vector		vecMaxs;		//0x0124
	float		flRadius;		//0x0130
	std::byte	pad1[0x1C];		//0x0134
};