#pragma once

// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/icliententitylist.h
class IClientEntityList
{
public:
public:
	int GetHighestEntityIndex(void)
	{
		typedef int(__thiscall* oGetHighestIndex)(void*);

		return GetVFunc<oGetHighestIndex>(this, 6)(this);
	}

	IClientNetworkable* GetClientNetworkable(int index)
	{
		typedef IClientNetworkable* (__thiscall* GetClientNetworkableFn)(void*, int);
		static GetClientNetworkableFn oGetClientNetworkable = GetVFunc<GetClientNetworkableFn>(this, 0);

		return oGetClientNetworkable(this, index);
	}
	Entity* GetClientEntity(int index)
	{
		typedef Entity* (__thiscall* oClientEnt)(void*, int);

		return GetVFunc<oClientEnt>(this, 3)(this, index);
	}

	Entity* GetClientEntityFromHandle(HANDLE handle)
	{
		typedef Entity* (__thiscall* oGetClientEntityFromHandle)(void*, HANDLE);

		return GetVFunc<oGetClientEntityFromHandle>(this, 4)(this, handle);
	}
};
