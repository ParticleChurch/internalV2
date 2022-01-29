#pragma once
//https://www.unknowncheats.me/wiki/Counter_Strike_Global_Offensive:Skin_Changer
class Skinchanger
{
public:
	struct EconomyItemCfg {
		int nFallbackPaintKit = 0;
		int nFallbackSeed = 0;
		int nFallbackStatTrak = -1;
		int iEntityQuality = 4;
		char* szCustomName = nullptr;
		float flFallbackWear = 0.1f;
	};
private:
	void Init();
	bool ApplyCustomSkin(Entity* pWeapon);
public:
	std::unordered_map<int, EconomyItemCfg> g_SkinChangerCfg;
	void FSN(int stage);
};

extern Skinchanger* skins;