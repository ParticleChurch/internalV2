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
	struct Menu
	{
		Color AccentColor = Color(0, 114, 255, 255);
		Color MainTextColor = Color(255, 255, 255, 255);
		Color SelectTextColor = Color(255, 255, 255, 255);
		Color BackgroundColor = Color(36, 36, 36, 154);
		Color WindowColor = Color(0, 0, 0, 51);
	};

	struct Movement
	{
		// bhop
		bool Bunnyhop = true;
		int Chance = 100;	// 100 means 100% of the time it works
		int MinHops = 0;	// zero means infinite
		int MaxHops = 0;	// zero means infinite

		// autostrafe
		bool Autostrafe = true;
		float AngSacrifice = 5.f;
		float StrafeAng = 5.f;

		// slowwalk
		float SlowWalkSpeed = 100.f;
	};

	struct Backtrack
	{
		bool Enable = true;
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
		bool SpectatorList = false;
		float SpecOpacity = 1.f;
		bool NoScope = false;
	};

	struct Performance {
		int HitchanceScan = 25;
		int MaxAimbotScan = 5;
		bool ShowConsole = false;
	};

	struct Antiaim {
		bool Enable = false;		// Enable
		//Yawbase stuff (kinda)
		int YawBase = 0;			// 180, AtTarget, Freestanding
		float LeftBaseOffset = 0.f;	// left yawbase offset
		float RightBaseOffset = 0.f;// right yawbase offset
		//Desync stuff mainly (kinda)
		int AAType = 0;				// Static, Sway, Opposite, Jitter, Random, Spin
		float LeftLimit = 58;		// Left limit
		float RightLimit = 58;		// Right limit
		float SwayStep = 1.f;			// sway step
		float JitterTime = 10.f;		// jitter time
		float RandomUpdateTime = 10.f;	// random update time
		float SpinStep = 1.f;			// spin step
		bool AntiBrute = false;		// Antibrute
	};

	struct Aimbot
	{
		float Hitchance = 66.f;
		int MinDamage = 1;
		int Priority = 0; // 0 is none
		bool Hitboxes[13] = { false, false, true, true, true, true, true, false, false, false, false, false, false };
		float HeadPointScale = 45.f;
		float BodyPointScale = 70.f;
	};

	struct Fakelag
	{
		int LagTicks = 1;
		int RandomTicks = 0;
	};
}

class Config
{
public:
	std::map<std::string, Keybind> Keybinds;

	// special stuff
	CFG::Menu menu;
	CFG::Performance performance;

	// cheat stuff
	CFG::Movement movement;
	CFG::Backtrack backtrack;
	CFG::ESP esp;
	CFG::Visuals vis;
	CFG::Antiaim aa;
	CFG::Aimbot aimbot;
	CFG::Fakelag fakelag;

	Config();
	void HandleKeybinds();
};

extern Config* cfg;
