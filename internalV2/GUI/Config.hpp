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
		bool Autostrafe = true;
		float AngSacrifice = 10.f;
		float StrafeAng = 5.f;
	};

	struct Offense
	{
		bool Backtrack = false;
		float BacktrackTime = 0.f;
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
		bool Grenade = false;
		Color GrenadeColor = Color();
	};

	struct Aimbot
	{
		//KeyBoundBool Enable = { 0,0,"Enable Aimbot", false };
		float Hitchance = 0.f;
		int MinDamage = 1;
		int Priority = 0; // 0 is none
		bool Hitboxes[13] = { false, false, false, false, false, false, false, false, false, false, false, false, false };
		float HeadPointScale = 0.f;
		float BodyPointScale = 0.f;
		float LimbPointScale = 0.f;
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
		bool Enable = true;
	};
}

class Config
{
public:
	std::map<std::string, Keybind> Keybinds;

	CFG::Movement movement;
	CFG::Offense offense;
	CFG::ESP esp;
	CFG::Aimbot aimbot;
	CFG::Performance performance;
	CFG::Visuals vis;
	CFG::Antiaim aa;

	Config();
	void HandleKeybinds();
};

extern Config* cfg;
