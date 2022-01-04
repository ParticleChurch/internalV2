#include "../Include.hpp"

Config* cfg = new Config();

class CHudElement {
public:
	bool IsActive()
	{
		return *(bool*)((DWORD)this + 13);
	}
};

CHudElement* FindHudElement(const char* Name)
{
	static void* PHUD = *(void**)(FindPattern("client.dll", "B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08") + 1);
	static auto func = (CHudElement * (__thiscall*)(void*, const char*))FindPattern("client.dll", "55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28");
	return func(PHUD, Name);
}

void Config::HandleKeybinds()
{
	CHudElement* chat = FindHudElement("CCSGO_HudChat");
	bool chatOpen = chat && chat->IsActive();
	// dont let any keybinds happen if the chat/console is open
	bool IsTyping = /*GUI::WantKeyboard ||*/ chatOpen || I::engine->IsConsoleVisible();

	// deal with keybinds
	for (auto& pair : cfg->Keybinds)
	{
		int VK = pair.second.key;

		// if not bound to a key, then don't bother changing anything
		if (VK == 0)
			continue;

		bool prevKeyPressed = pair.second.keypressed;
		bool curKeyPressed = GetAsyncKeyState(VK) < 0;
		pair.second.keypressed = curKeyPressed;

		// hold to enable
		if (pair.second.mode == 0)
		{
			pair.second.boolean = curKeyPressed;
		}
		// toggle
		else if (pair.second.mode == 1)
		{
			if (curKeyPressed && prevKeyPressed != curKeyPressed)
				pair.second.boolean = !pair.second.boolean;
		}
		// hold to disable
		else if (pair.second.mode == 2)
		{
			pair.second.boolean = !curKeyPressed;
		}
	}
}

Config::Config()
{
	Keybind Aimbot = { false, false, "Aimbot", 0, 0 };
	Keybinds.insert(std::make_pair(Aimbot.name, Aimbot));

	Keybind ThirdPerson{ false, false, "Thirdperson", 1, 0 };
	Keybinds.insert(std::make_pair(ThirdPerson.name, ThirdPerson));

	Keybind SlowWalk = { false, false, "Slowwalk", 0, 0 };
	Keybinds.insert(std::make_pair(SlowWalk.name, SlowWalk));

	Keybind InvertAA = { false, false, "Invert AA", 0, 0 };
	Keybinds.insert(std::make_pair(InvertAA.name, InvertAA));

}
