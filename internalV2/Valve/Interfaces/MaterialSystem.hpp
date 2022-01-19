#pragma once
class RenderContext;
class KeyValues;

class MaterialSystem {
public:
    Material* FindMaterial(const char* materialName) noexcept
    {
        typedef Material* (__thiscall* oFindMaterial)(void*, char const*, const char*, bool, const char*);
        return GetVFunc<oFindMaterial>(this, 84)(this, materialName, nullptr, true, nullptr);
    }

    short FirstMaterial() noexcept
    {
        typedef short(__thiscall* ofirstMaterial)(void*);
        return GetVFunc<ofirstMaterial>(this, 86)(this);
    }

    short NextMaterial(short handle) noexcept
    {
        typedef short(__thiscall* onextMaterial)(void*, short);
        return GetVFunc<onextMaterial>(this, 87)(this, handle);
    }

    short InvalidMaterial() noexcept
    {
        typedef short(__thiscall* onextMaterial)(void*);
        return GetVFunc<onextMaterial>(this, 88)(this);
    }

    Material* GetMaterial(short handle) noexcept
    {
        typedef Material* (__thiscall* ogetMaterial)(void*, short);
        return GetVFunc<ogetMaterial>(this, 89)(this, handle);
    }

    RenderContext* GetRenderContext() noexcept
    {
        typedef RenderContext* (__thiscall* ogetRenderContext)(void*);
        return GetVFunc<ogetRenderContext>(this, 115)(this);
    }

    Material* CreateMaterial(const char* materialName, KeyValues* keyValues) noexcept
    {
        typedef Material* (__thiscall* ocreateMaterial)(void*, const char*, KeyValues*);
        return GetVFunc<ocreateMaterial>(this, 83)(this, materialName, keyValues);
    }
};