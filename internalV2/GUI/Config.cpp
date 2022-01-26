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

void Config::SaveConfig(std::string filename)
{
	std::fstream clr;
	clr.open(filename, std::ofstream::out | std::ofstream::trunc);
	clr.close();

	// open in append mode and write data to it :D
	std::fstream file_obj;
	file_obj.open(filename, std::ios::app);
	file_obj.write((char*)&menu, sizeof(CFG::Menu));
	file_obj.write((char*)&movement , sizeof(CFG::Movement))		   ;
	file_obj.write((char*)&backtrack , sizeof(CFG::Backtrack))		   ;
	file_obj.write((char*)&esp , sizeof(CFG::ESP))					   ;
	file_obj.write((char*)&vis , sizeof(CFG::Visuals))				   ;
	file_obj.write((char*)&aa , sizeof(CFG::Antiaim))				   ;
	file_obj.write((char*)&aimbot , sizeof(CFG::Aimbot))			   ;
	file_obj.write((char*)&fakelag , sizeof(CFG::Fakelag))			   ;
	file_obj.write((char*)&chams , sizeof(CFG::Chams))				   ;
	file_obj.write((char*)&world_vis , sizeof(CFG::WorldVisuals))	   ;
	file_obj.write((char*)&glow , sizeof(CFG::Glow			))		   ;
	file_obj.write((char*)&triggerbot, sizeof(CFG::Triggerbot));
	file_obj.close();
}

void Config::OpenConfig(std::string filename)
{
	// read data from text file
	std::fstream file_obj;
	file_obj.open(filename, std::ios::in);
	file_obj.seekg(0);
	file_obj.read((char*)&menu, sizeof(CFG::Menu));
	file_obj.read((char*)&this->movement, sizeof(CFG::Movement));
	file_obj.read((char*)&this->backtrack, sizeof(CFG::Backtrack));
	file_obj.read((char*)&this->esp, sizeof(CFG::ESP));
	file_obj.read((char*)&this->vis, sizeof(CFG::Visuals));
	file_obj.read((char*)&this->aa, sizeof(CFG::Antiaim));
	file_obj.read((char*)&this->aimbot, sizeof(CFG::Aimbot));
	file_obj.read((char*)&this->fakelag, sizeof(CFG::Fakelag));
	file_obj.read((char*)&this->chams, sizeof(CFG::Chams));
	file_obj.read((char*)&this->world_vis, sizeof(CFG::WorldVisuals));
	file_obj.read((char*)&this->glow, sizeof(CFG::Glow));
	file_obj.read((char*)&this->triggerbot, sizeof(CFG::Triggerbot));
	file_obj.close();
}

void Config::DeleteConfig(std::string filename)
{
	if (remove(filename.c_str()) != 0)
		return; // valid remove
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

	Keybind FakeDuck = { false, false, "Fake Duck", 0, 0 };
	Keybinds.insert(std::make_pair(FakeDuck.name, FakeDuck));

	Keybind Triggerbot = { false, false, "Triggerbot", 0, 0 };
	Keybinds.insert(std::make_pair(Triggerbot.name, Triggerbot));

}

bool CFG::GetVal(std::string total, std::string str, std::string& substr)
{
	std::size_t start = total.find(str);
	if (start == std::string::npos)
		return false;
	std::size_t end = total.find("|", str.length());
	if (end == std::string::npos)
		return false;
	substr = total.substr(start + str.length(), end);

	return true;
}
