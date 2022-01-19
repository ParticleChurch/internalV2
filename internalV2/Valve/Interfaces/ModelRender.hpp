#pragma once
class ModelRender
{
public:
    void ForcedMaterialOverride(Material* mat)
    {
        typedef void(__thiscall* oForcedMaterialOverride)(void*, Material*, int, int);
        return GetVFunc<oForcedMaterialOverride>(this, 1)(this, mat, 0, 0);
    }
};