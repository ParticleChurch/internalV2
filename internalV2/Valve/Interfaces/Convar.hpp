#pragma once

using FnCommandCallbackV1_t = void(__cdecl*)();
using FnChangeCallback_t = void(__cdecl*)(void*, const char*, float);
class CConVar
{
public:
	const char* GetName()
	{
		return CallVFunc<const char*>(this, 5);
	}

	float GetFloat() // idx @ 11
	{
		std::uint32_t uXored = *reinterpret_cast<std::uint32_t*>(&pParent->flValue) ^ reinterpret_cast<std::uint32_t>(this);
		return *reinterpret_cast<float*>(&uXored);
	}

	int GetInt() // idx @ 12
	{
		return static_cast<int>(pParent->iValue ^ reinterpret_cast<int>(this));
	}

	bool GetBool() // idx @ 13
	{
		return !!GetInt();
	}

	const char* GetString() const
	{
		char const* szValue = pParent->szString;
		return szValue ? szValue : "";
	}

	void SetValue(const char* szValue)
	{
		return CallVFunc<void>(this, 14, szValue);
	}

	void SetValue(float flValue)
	{
		return CallVFunc<void>(this, 15, flValue);
	}

	void SetValue(int iValue)
	{
		return CallVFunc<void>(this, 16, iValue);
	}

	void SetValue(Color colValue)
	{
		return CallVFunc<void>(this, 17, colValue);
	}

public:
	std::byte						pad0[0x4];			//0x00
	CConVar* pNext;				//0x04
	bool							bRegistered;		//0x08
	const char* szName;				//0x0C
	const char* szHelpString;		//0x10
	int								nFlags;				//0x14
	FnCommandCallbackV1_t			pCallback;			//0x18
	CConVar* pParent;			//0x1C
	const char* szDefaultValue;		//0x20
	char* szString;			//0x24
	int								iStringLength;		//0x28
	float							flValue;			//0x2C
	int								iValue;				//0x30
	bool							bHasMin;			//0x34
	float							flMinValue;			//0x38
	bool							bHasMax;			//0x3C
	float							flMaxValue;			//0x40
	CUtlVector<FnChangeCallback_t>	fnChangeCallbacks;	//0x44
};

class IAppSystem
{
private:
	virtual void function0() = 0;
	virtual void function1() = 0;
	virtual void function2() = 0;
	virtual void function3() = 0;
	virtual void function4() = 0;
	virtual void function5() = 0;
	virtual void function6() = 0;
	virtual void function7() = 0;
	virtual void function8() = 0;
};

using CVarDLLIdentifier_t = int;
class CConBase;
class CConVar;
class CConCmd;
class IConVar : public IAppSystem
{
public:
	virtual CVarDLLIdentifier_t	AllocateDLLIdentifier() = 0;
	virtual void			RegisterConCommand(CConVar* pCommandBase, int iDefaultValue = 1) = 0;
	virtual void			UnregisterConCommand(CConVar* pCommandBase) = 0;
	virtual void			UnregisterConCommands(CVarDLLIdentifier_t id) = 0;
	virtual const char* GetCommandLineValue(const char* szVariableName) = 0;
	virtual CConBase* FindCommandBase(const char* szName) = 0;
	virtual const CConBase* FindCommandBase(const char* szName) const = 0;
	virtual CConVar* FindVar(const char* szVariableName) = 0;
	virtual const CConVar* FindVar(const char* szVariableName) const = 0;
	virtual CConCmd* FindCommand(const char* szName) = 0;
	virtual const CConCmd* FindCommand(const char* szName) const = 0;
	virtual void			InstallGlobalChangeCallback(FnChangeCallback_t callback) = 0;
	virtual void			RemoveGlobalChangeCallback(FnChangeCallback_t callback) = 0;
	virtual void			CallGlobalChangeCallbacks(CConVar* pVar, const char* szOldString, float flOldValue) = 0;
	virtual void			InstallConsoleDisplayFunc(void* pDisplayFunc) = 0;
	virtual void			RemoveConsoleDisplayFunc(void* pDisplayFunc) = 0;
	virtual void			ConsoleColorPrintf(const Color& color, const char* pFormat, ...) const = 0;
	virtual void			ConsolePrintf(const char* pFormat, ...) const = 0;
	virtual void			ConsoleDPrintf(const char* pFormat, ...) const = 0;
	virtual void			RevertFlaggedConVars(int nFlag) = 0;
};