#include "Include.hpp"

namespace I
{
	//InterfaceTools* NewInterface = new InterfaceTools;
	IBaseClientDll* client = nullptr;

	void GUIInit()
	{
		
	}

	void Init()
	{
		//InterfaceTools* NewInterface = new InterfaceTools;
		client = GetInterface<IBaseClientDll>("client.dll", "VClient018");
		
	}
}