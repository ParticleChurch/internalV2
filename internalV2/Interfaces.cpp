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

	void Init()
	{
		engine = GetInterface<IEngineClient      >("engine.dll", "VEngineClient014");
		surface = GetInterface<ISurface          >("vguimatsurface.dll", "VGUI_Surface031");
		client = GetInterface<IBaseClientDll>("client.dll", "VClient018");
		clientmode = **(IClientModeShared***)((*(uintptr_t**)(client))[10] + 0x5);
		panel = GetInterface<IVPanel               >("vgui2.dll", "VGUI_Panel009");
		entitylist = GetInterface<IClientEntityList   >("client.dll", "VClientEntityList003");
		inputsystem = GetInterface<IInputSystem         >("inputsystem.dll", "InputSystemVersion001");
	}
}