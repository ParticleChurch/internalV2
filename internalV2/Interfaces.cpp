#include "Include.hpp"

namespace I
{
	//InterfaceTools* NewInterface = new InterfaceTools;
	IBaseClientDll* client = nullptr;
	IEngineClient* engine = nullptr;
	ISurface* surface = nullptr;

	void GUIInit()
	{
		engine = GetInterface<IEngineClient      >("engine.dll", "VEngineClient014");
		surface = GetInterface<ISurface          >("vguimatsurface.dll", "VGUI_Surface031");
	}

	void Init()
	{
		//InterfaceTools* NewInterface = new InterfaceTools;
		client = GetInterface<IBaseClientDll>("client.dll", "VClient018");
		
	}
}