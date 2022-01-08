#pragma once

// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/cdll_int.h

class IClientUnknown;
class ClientClass;
class bf_read;
class CBaseHandle;

class IHandleEntity
{
public:
	virtual ~IHandleEntity() {}
	virtual void SetRefEHandle(const CBaseHandle& handle) = 0;
	virtual const CBaseHandle& GetRefEHandle() const = 0;
};

class IClientNetworkable;
class IClientRenderable;
class IClientEntity;
class IClientThinkable;
class IClientAlphaProperty;

class IClientNetworkable
{
public:
	virtual IClientUnknown* GetIClientUnknown() = 0;
	virtual void             Release() = 0;
	virtual ClientClass* GetClientClass() = 0;
	virtual void             NotifyShouldTransmit(int state) = 0;
	virtual void             OnPreDataChanged(int updateType) = 0;
	virtual void             OnDataChanged(int updateType) = 0;
	virtual void             PreDataUpdate(int updateType) = 0;
	virtual void             PostDataUpdate(int updateType) = 0;
	virtual void             __unkn(void) = 0;
	virtual bool             IsDormant(void) = 0;
	virtual int              EntIndex(void) const = 0;
	virtual void             ReceiveMessage(int classID, bf_read& msg) = 0;
	virtual void* GetDataTableBasePtr() = 0;
	virtual void             SetDestroyedOnRecreateEntities(void) = 0;
};

class IClientUnknown : public IHandleEntity
{
public:
	virtual ICollideable* GetCollideable() = 0;
	virtual IClientNetworkable* GetClientNetworkable() = 0;
	virtual IClientRenderable* GetClientRenderable() = 0;
	virtual IClientEntity* GetIClientEntity() = 0;
	virtual Entity* GetBaseEntity() = 0;
	virtual IClientThinkable* GetClientThinkable() = 0;
	//virtual IClientModelRenderable*  GetClientModelRenderable() = 0;
	virtual IClientAlphaProperty* GetClientAlphaProperty() = 0;
};

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
