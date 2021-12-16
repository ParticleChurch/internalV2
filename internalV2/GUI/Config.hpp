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
		bool Bunnyhop = true;
		int Chance = 100;	// 100 means 100% of the time it works
		int MinHops = 0;	// zero means infinite
		int MaxHops = 0;	// zero means infinite

		// autostrafe
		bool Autostrafe = false;
		float AngSacrifice = 10.f;
		float StrafeAng = 5.f;
	};

	struct Offense
	{
		bool Backtrack = true;
		float BacktrackTime = 200.f;
	};

	struct ESP
	{
		bool Enable = true;
		bool Name = true;
		Color NameColor = Color();
		bool Skeleton = true;
		Color SkeletonColor = Color();
		bool Health = true;
		bool Weapon = true;
		Color WeaponColor = Color();
	};

	struct Visuals {
		float ThirdPersonDistance = 150.f;
	};

	struct Performance {
		int HitchanceScan = 25;
		int MaxAimbotScan = 5;
		bool ShowConsole = false;
	};

	struct Antiaim {
		bool Enable = false;
	};
}

class Config
{
public:
	std::map<std::string, Keybind> Keybinds;

	CFG::Movement movement;
	CFG::Offense offense;
	CFG::ESP esp;
	CFG::Performance performance;
	CFG::Visuals vis;
	CFG::Antiaim aa;

	Config();
	void HandleKeybinds();
};

extern Config* cfg;
