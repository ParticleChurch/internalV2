#include "Include.hpp"

namespace I
{
	//InterfaceTools* NewInterface = new InterfaceTools;
	IBaseClientDll* client = nullptr;
	IEngineClient* engine = nullptr;
	ISurface* surface = nullptr;
	IClientModeShared* clientmode = nullptr;
	IVPanel* panel = nullptr;
	IClientEntityList* entitylist = nullptr;
	IInputSystem* inputsystem = nullptr;
	IWeaponSystem* weaponsystem = nullptr;
	IPhysicsSurfaceProps* physicsprops = nullptr;
	IGlobalVarsBase* globalvars = nullptr;
	IEngineTrace* enginetrace = nullptr;
	IConVar* convar = nullptr;
	IVModelInfo* modelinfo = nullptr;
	IInput* input = nullptr;
	IVDebugOverlay* debugoverlay = nullptr;
	IClientState* clientstate = nullptr;
	CEffects* effects = nullptr;
	GameEventManager* gameeventmanager = nullptr;

	void Init()
	{
		engine = GetInterface<IEngineClient      >("engine.dll", "VEngineClient014");
		surface = GetInterface<ISurface          >("vguimatsurface.dll", "VGUI_Surface031");
		client = GetInterface<IBaseClientDll>("client.dll", "VClient018");
		clientmode = **(IClientModeShared***)((*(uintptr_t**)(client))[10] + 0x5);
		panel = GetInterface<IVPanel               >("vgui2.dll", "VGUI_Panel009");
		entitylist = GetInterface<IClientEntityList   >("client.dll", "VClientEntityList003");
		inputsystem = GetInterface<IInputSystem         >("inputsystem.dll", "InputSystemVersion001");
		weaponsystem = *reinterpret_cast<IWeaponSystem**>(FindPattern("client.dll", "8B 35 ? ? ? ? FF 10 0F B7 C0") + 0x2);
		engine = GetInterface<IEngineClient      >("engine.dll", "VEngineClient014");
		physicsprops = GetInterface<IPhysicsSurfaceProps >("vphysics.dll", "VPhysicsSurfaceProps001");
		globalvars = **(IGlobalVarsBase***)((*(uintptr_t**)(client))[11] + 10);
		enginetrace = GetInterface<IEngineTrace        >("engine.dll", "EngineTraceClient004");
		convar = GetInterface<IConVar                >("vstdlib.dll", "VEngineCvar007");
		modelinfo = GetInterface<IVModelInfo         >("engine.dll", "VModelInfoClient004");
		input = *(IInput**)((*(DWORD**)client)[16] + 0x1);
		debugoverlay = GetInterface<IVDebugOverlay      >("engine.dll", "VDebugOverlay004");
		//bool sample = GetInterface2<bool>("materialsystem.dll", "CCommonHostState", true);
		clientstate = **reinterpret_cast<IClientState***>(FindPattern("engine.dll", "A1 ? ? ? ? 8B 88 ? ? ? ? 85 C9 75 07") + 0x1);
		//= **reinterpret_cast<ClientState***>(FindPattern("engine.dll", "A1 ? ? ? ? 8B 80 ? ? ? ? C3") + 1);
		effects = GetInterface2<CEffects>("engine.dll", "VEngineEffects0", false); // i got lazy lol
		gameeventmanager = GetInterface<GameEventManager    >("engine.dll", "GAMEEVENTSMANAGER002");

	}
}