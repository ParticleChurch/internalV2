#pragma once

// this is for keybinds
struct Keybind
{
	bool boolean = false;
	bool keypressed = false;
	std::string name = "";
	int mode = 0; // 0 hold to enable, 1 toggle, 2 hold to disable
	int key = 0;
};

// this namespace is purely for organization of the structs, not used as something actually used
namespace CFG
{
	struct Movement
	{
		// bhop
		bool Bunnyhop = false;
		int Chance = 100;	// 100 means 100% of the time it works
		int MinHops = 0;	// zero means infinite
		int MaxHops = 0;	// zero means infinite

		// autostrafe
		bool Autostrafe = false;
		float AngSacrifice = 0.f;
		float StrafeAng = 5.f;
	};
}

class Config
{
public:
	std::map<std::string, Keybind> Keybinds;

	CFG::Movement movement;

	Config();
	void HandleKeybinds();
};

extern Config* cfg;
