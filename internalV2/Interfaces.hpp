#pragma once
class InterfaceReg
{
private:
	using InstantiateInterfaceFn = void* (*)();
public:
	InstantiateInterfaceFn m_CreateFn;
	const char* m_pName;
	InterfaceReg* m_pNext;
};

namespace I
{
	//interfaces
	extern IBaseClientDll* client;
	extern IEngineClient* engine;
	extern ISurface* surface;
	extern IClientModeShared* clientmode;
	extern IVPanel* panel;
	extern IClientEntityList* entitylist;
	extern IInputSystem* inputsystem;
	extern IWeaponSystem* weaponsystem;
	extern IPhysicsSurfaceProps* physicsprops;
	extern IGlobalVarsBase* globalvars;
	extern IEngineTrace* enginetrace;
	extern IConVar* convar;
	extern IVModelInfo* modelinfo;
	extern IInput* input;
	extern IVDebugOverlay* debugoverlay;
	extern IClientState* clientstate;

	// func
	typedef void* (*InterfaceCreator)(const char*, int);
	template <typename T>
	T* GetInterface(const char* MoudleName, const char* InterfaceName)
	{
		InterfaceCreator createInterface = (InterfaceCreator)GetProcAddress(GetModuleHandleA(MoudleName), "CreateInterface");
		return (T*)createInterface(InterfaceName, 0);
	}

	template<typename T>
	static T* GetInterface2(const char* mod_name, const char* interface_name, bool exact = false) {
		T* iface = nullptr;
		InterfaceReg* register_list;
		int part_match_len = strlen(interface_name); //-V103

		DWORD interface_fn = reinterpret_cast<DWORD>(GetProcAddress(GetModuleHandleA(mod_name), "CreateInterface"));

		if (!interface_fn) {
			return nullptr;
		}

		unsigned int jump_start = (unsigned int)(interface_fn)+4;
		unsigned int jump_target = jump_start + *(unsigned int*)(jump_start + 1) + 5;

		register_list = **reinterpret_cast<InterfaceReg***>(jump_target + 6);

		for (InterfaceReg* cur = register_list; cur; cur = cur->m_pNext) {
			//L::Debug((std::string(mod_name) + std::string(" ") + cur->m_pName).c_str());
			if (exact == true) {
				if (strcmp(cur->m_pName, interface_name) == 0)
					iface = reinterpret_cast<T*>(cur->m_CreateFn());
			}
			else {
				if (!strncmp(cur->m_pName, interface_name, part_match_len) && std::atoi(cur->m_pName + part_match_len) > 0) //-V106
					iface = reinterpret_cast<T*>(cur->m_CreateFn());
			}
		}
		return iface;
	}

	// idk why we would need a gui init/free :/
	extern void Init();
}