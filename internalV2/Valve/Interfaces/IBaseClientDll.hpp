#pragma once

// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/cdll_int.h

class IClientNetworkable;
using CreateClientClassFn = IClientNetworkable * (__cdecl*)(int, int);
using CreateEventFn = IClientNetworkable * (__cdecl*)();

class CStandartRecvProxies;
struct RecvTable_t;
enum class EClassIndex;

// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/client_class.h
class CBaseClient
{
public:
	CreateClientClassFn		pCreateFn;
	CreateEventFn			pCreateEventFn;
	char* szNetworkName;
	RecvTable_t* pRecvTable;
	CBaseClient* pNext;
	EClassIndex				nClassID;
};

using CreateInterfaceFn = void* (__cdecl*)(const char*, int*);
using InstantiateInterfaceFn = void* (__cdecl*)();
using DemoCustomDataCallbackFn = void(__cdecl*)(unsigned char*, std::size_t);

class IBaseClientDll
{
public:
	CBaseClient* GetAllClasses()
	{
		return CallVFunc<CBaseClient*>(this, 8);
	}

	bool DispatchUserMessage(int iMessageType, unsigned int nFlags, int iSize, const void* pMessageData)
	{
		return CallVFunc<bool>(this, 38, iMessageType, nFlags, iSize, pMessageData);
	}

	CStandartRecvProxies* GetStandardRecvProxies()
	{
		return CallVFunc<CStandartRecvProxies*>(this, 48);
	}

	//ButtonCode_t
	//https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/inputsystem/ButtonCode.h
	bool IN_KeyEvent(int eventcode, int keynum, const char* pszCurrentBinding) noexcept
	{
		typedef bool(__thiscall* oIN_KeyEvent)(void*, int, int, const char*);
		return GetVFunc< oIN_KeyEvent >(this, 23)(this, eventcode, keynum, pszCurrentBinding);
	}

	//https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/cdll_int.h#L632
	// If key is found by name, returns whether it's being held down in isdown, otherwise function returns false
	//virtual bool			IN_IsKeyDown(const char* name, bool& isdown) = 0;
	// Notify the client that the mouse was wheeled while in game - called prior to executing any bound commands.
	//virtual void			IN_OnMouseWheeled(int nDelta) = 0;
	// Raw keyboard signal, if the client .dll returns 1, the engine processes the key as usual, otherwise,
	//  if the client .dll returns 0, the key is swallowed.
	//virtual int				IN_KeyEvent(int eventcode, ButtonCode_t keynum, const char* pszCurrentBinding) = 0;
};
