#pragma once
typedef unsigned long HCursor;
class IHudChat;
class IClientModeShared
{
public:
	std::byte	pad0[0x1B];
	void* pViewport;
	IHudChat* pChatElement;
	HCursor		hCursorNone;
	void* pWeaponSelection;
	int			nRootSize[2];
};