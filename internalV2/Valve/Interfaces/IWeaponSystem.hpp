#pragma once
class IWeaponSystem
{
public:
	CCSWeaponData* GetWeaponData(short nItemDefinitionIndex)
	{
		return CallVFunc<CCSWeaponData*>(this, 2, nItemDefinitionIndex);
	}
};