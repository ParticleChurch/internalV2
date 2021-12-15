#pragma once
struct Model_t
{
	void* handle;
	char		szName[260];	//0x0004
	int			nLoadFlags;		//0x0108
	int			nServerCount;	//0x010C
	int			iType;			//0x0110
	int			uFlags;			//0x0114
	Vector		vecMins;		//0x0118
	Vector		vecMaxs;		//0x0124
};