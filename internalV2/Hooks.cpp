#include "Include.hpp"

#define DEBUG_HOOKS true

//for windprc hook
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

template<typename ...Args>
void ConsoleColorMsg(const Color& color, const char* fmt, Args ...args)
{
	using ConColorMsg = void(*)(const Color&, const char*, ...);
	static ConColorMsg con_color_msg = nullptr;
	if (!con_color_msg) {
		con_color_msg = reinterpret_cast<ConColorMsg>(GetProcAddress(
			GetModuleHandleA("tier0.dll"),
			"?ConColorMsg@@YAXABVColor@@PBDZZ")
			);
	}

	con_color_msg(color, fmt, args...);
}

namespace H
{
	// VMT Managers
	VMT::Manager* d3d9VMT = nullptr;
	VMT::Manager* surfaceVMT = nullptr;
	VMT::Manager* clientVMT = nullptr;
	VMT::Manager* clientmodeVMT = nullptr;
	VMT::Manager* panelVMT = nullptr;
	VMT::Manager* inputVMT = nullptr;

	// Original Functions
	Reset oReset;
	LockCursor oLockCursor;
	EndScene oEndScene;
	WNDPROC oWndProc = NULL;
	FrameStageNotify oFrameStageNotify;
	CreateMove oCreateMove;
	PaintTraverse oPaintTraverse;
	CamToFirstPeron oCamToFirstPeron;
	DoPostScreenEffects oDoPostScreenEffects;
	
	// special GUI Vars
	bool D3dInit = false;
	HWND CSGOWindow = NULL;
	std::vector<std::string> console;
}

void H::Init()
{
	L::Debug("Initializing Hooks");

	L::Debug("-->Special hook initializations");
	// special initializations
	DWORD pD3d9DevicePattern;
	while (!(pD3d9DevicePattern = FindPattern("shaderapidx9.dll", "A1 ? ? ? ? 50 8B 08 FF 51 0C")));
	PDWORD pD3d9Device = *(PDWORD*)(pD3d9DevicePattern + 1);
	IDirect3DDevice9* D3d9Device = (IDirect3DDevice9*)*pD3d9Device;
	while (!(CSGOWindow = FindWindowW(L"Valve001", nullptr))) Sleep(10);

	// do VMTs
	L::Debug("-->VMT's");
	d3d9VMT = new VMT::Manager((VMT::VMT*)D3d9Device);
	surfaceVMT = new VMT::Manager((VMT::VMT*)I::surface);
	clientVMT = new  VMT::Manager((VMT::VMT*)I::client);
	clientmodeVMT = new VMT::Manager((VMT::VMT*)I::clientmode);
	panelVMT = new VMT::Manager((VMT::VMT*)I::panel);
	inputVMT = new VMT::Manager((VMT::VMT*)I::input);

	// do hooks
	L::Debug("-->oReset");
	oReset = (Reset)d3d9VMT->Hook(16, &ResetHook);
	L::Debug("-->LockCursorHook");
	oLockCursor = (LockCursor)surfaceVMT->Hook(67, &LockCursorHook);
	L::Debug("-->EndSceneHook");
	oEndScene = (EndScene)d3d9VMT->Hook(42, &EndSceneHook);
	L::Debug("-->WndProc");
	oWndProc = WNDPROC(SetWindowLongPtrW(CSGOWindow, GWLP_WNDPROC, LONG_PTR(WndProc)));
	L::Debug("-->FrameStageNotify");
	oFrameStageNotify = (FrameStageNotify)clientVMT->Hook(37, &FrameStageNotifyHook);
	L::Debug("-->CreateMove");
	oCreateMove = (CreateMove)clientmodeVMT->Hook(24, &CreateMoveHook);
	L::Debug("-->PaintTraverse");
	oPaintTraverse = (PaintTraverse)panelVMT->Hook(41, &PaintTraverseHook);
	L::Debug("-->CamToFirstPeron");
	oCamToFirstPeron = (CamToFirstPeron)inputVMT->Hook(36, &CamToFirstPeronHook);
	L::Debug("-->DoPostScreenEffects");
	oDoPostScreenEffects = (DoPostScreenEffects)clientmodeVMT->Hook(44, &DoPostScreenEffectsHook);
}

void H::Free()
{
	L::Debug("Freeing Hooks");

	// restore the VMTs to normal
	if (d3d9VMT) d3d9VMT->UnhookAll();
	if (surfaceVMT) surfaceVMT->UnhookAll();
	if (clientVMT) clientVMT->UnhookAll();
	if (clientmodeVMT) clientmodeVMT->UnhookAll();
	if (panelVMT) panelVMT->UnhookAll();
	// get user out of thirdperson
	oCamToFirstPeron(I::input);
	if (inputVMT) inputVMT->UnhookAll();

	// do some last minute adjustments
	// will do later lol

	L::Debug("Freeing VMTs");

	// free the VMTs now that they are useless
	if (d3d9VMT) delete d3d9VMT;
	if (surfaceVMT) delete surfaceVMT;
	if (clientVMT) delete clientVMT;
	if (clientmodeVMT) delete clientmodeVMT;
	if (panelVMT) delete panelVMT;
	if (inputVMT) delete inputVMT;

	L::Debug("Freeing Hacks");

	// now delete hacks
	delete autowall;
	delete movement;
	delete backtrack;
	delete lagcomp;
	delete esp;
	delete aimbot;
	delete thirdperson;

	// let user use mouse again?
	if (I::engine->IsInGame())
		I::surface->LockCursor();
}

long __stdcall H::ResetHook(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	//if  (DEBUG_HOOKS) L::Debug("ResetHook");

	ImGui_ImplDX9_InvalidateDeviceObjects();
	ImGui_ImplDX9_CreateDeviceObjects();

	auto ogOutput = oReset(device, pPresentationParameters);
	return ogOutput;
}

void __stdcall H::LockCursorHook()
{
	if (DEBUG_HOOKS) L::Debug("LockCursorHook");

	if (G::KillDLL)
		return oLockCursor(I::surface);

	if (G::MenuOpen) {
		return I::surface->UnLockCursor();
	}

	return oLockCursor(I::surface);
}

long __stdcall H::EndSceneHook(IDirect3DDevice9* device)
{
	//if (DEBUG_HOOKS) L::Debug("EndSceneHook");

	// init imgui
	if (!D3dInit) {
		D3dInit = true;

		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = nullptr;

		ImGui_ImplWin32_Init(CSGOWindow);
		ImGui_ImplDX9_Init(device);
	}

	// draw imgui 
	{
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		menu->Render();

		aimbot->RenderStats();
		lagcomp->RenderStats();
		
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	}

	return H::oEndScene(device);
}

LRESULT __stdcall H::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if  (DEBUG_HOOKS) L::Debug("WndProc");

	if (!D3dInit) return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);

	// determine input type
	bool KeyboardPress =
		uMsg == WM_SYSCHAR ||
		uMsg == WM_CHAR ||
		uMsg == WM_DEADCHAR ||
		uMsg == WM_HOTKEY ||
		uMsg == WM_KEYDOWN ||
		uMsg == WM_SYSDEADCHAR ||
		uMsg == WM_SYSKEYDOWN ||
		uMsg == WM_UNICHAR;
	bool KeyboardRelease =
		uMsg == WM_KEYUP ||
		uMsg == WM_SYSKEYUP;
	bool KeyboardInput = KeyboardPress || KeyboardRelease;

	bool MouseInput =
		uMsg == WM_MOUSEMOVE || uMsg == WM_INPUT || uMsg == WM_SETCURSOR ||
		uMsg == WM_MOUSEWHEEL || uMsg == WM_MOUSEHWHEEL ||
		uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDBLCLK ||
		uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDBLCLK ||
		uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONDBLCLK ||
		uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONDBLCLK ||
		uMsg == WM_XBUTTONDOWN || uMsg == WM_XBUTTONDBLCLK ||
		uMsg == WM_NCLBUTTONDOWN || uMsg == WM_NCLBUTTONDBLCLK ||
		uMsg == WM_NCRBUTTONDOWN || uMsg == WM_NCRBUTTONDBLCLK ||
		uMsg == WM_NCMBUTTONDOWN || uMsg == WM_NCMBUTTONDBLCLK ||
		uMsg == WM_NCXBUTTONDOWN || uMsg == WM_NCXBUTTONDBLCLK;
	bool UnknownInput = !KeyboardInput && !MouseInput;

	// insert key
	if (uMsg == WM_KEYDOWN) {
		if (wParam == VK_INSERT) {
			G::MenuOpen = !G::MenuOpen;
		}
	}

	// send input to imgui
	if (G::MenuOpen && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) {
		return true;
	}
	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

void __stdcall H::FrameStageNotifyHook(int stage)
{
	if  (DEBUG_HOOKS) L::Debug("FrameStageNotifyHook");

	G::IsInGame = I::engine->IsInGame();

	// Do menu input fix
	if (!G::IsInGame && G::MenuOpen)
		I::inputsystem->EnableInput(false);
	else
		I::inputsystem->EnableInput(true);

	if (G::IsInGame)
	{
		G::LocalplayerIndex = I::engine->GetLocalPlayer();
		G::Localplayer = I::entitylist->GetClientEntity(G::LocalplayerIndex);
		G::LocalplayerAlive = G::Localplayer ? G::Localplayer->GetHealth() > 0 && G::Localplayer->GetLifeState() == LIFE_ALIVE: false;
		G::LocalplayerTeam = G::Localplayer ? G::Localplayer->GetTeam() : 0;
		G::LocalplayerWeapon = G::Localplayer ? G::Localplayer->GetActiveWeapon() : nullptr;
		G::LocalplayerWeapondata = G::LocalplayerWeapon ? G::LocalplayerWeapon->GetWeaponData() : nullptr;
		
		if (!G::Localplayer || !G::LocalplayerAlive)
		{
			lagcomp->ClearRecords();
			return oFrameStageNotify(stage);
		}
	}
	else
	{
		lagcomp->ClearPlayerList();
		G::LocalplayerAlive = false;
		G::Localplayer = nullptr;
		G::LocalplayerIndex = 0;
		G::LocalplayerTeam = 0;
		G::LocalplayerWeapon = nullptr;
		G::LocalplayerWeapondata = nullptr;
		return oFrameStageNotify(stage);
	}

	

	static int deadflagOffset = N::GetOffset("DT_BasePlayer", "deadflag");
	if (stage == FRAME_RENDER_START) {
		if (cfg->Keybinds["Thirdperson"].boolean)
			*(QAngle*)((DWORD)G::Localplayer + deadflagOffset + 4) = G::EndAngle;
	}

	lagcomp->Run_FSN(stage);

	oFrameStageNotify(stage);
}

bool __stdcall H::CreateMoveHook(float flInputSampleTime, CUserCmd* cmd)
{
	if  (DEBUG_HOOKS) L::Debug("CreateMoveHook");

	bool oFunc = oCreateMove(flInputSampleTime, cmd);

	if (!oFunc || !cmd || !cmd->iCommandNumber)
	{
		/*return oFunc;*/
		return false;
	}

	if (I::engine->IsInGame() && cmd && G::Localplayer)
	{
		PVOID pebp;
		__asm mov pebp, ebp;
		bool* pSendPacket = (bool*)(*(DWORD*)pebp - 0x1C);
		bool& bSendPacket = *pSendPacket;

		static int LagVal = 0;
		static int LastTickCount = I::globalvars->iTickCount;
		if (LastTickCount != I::globalvars->iTickCount)
		{
			LastTickCount = I::globalvars->iTickCount;
			LagVal++;
		}

		if (LagVal >= 5)
		{
			LagVal = 0;
			bSendPacket = true;
		}
		else
			bSendPacket = false;

		// Menu fix
		if (G::MenuOpen && (cmd->iButtons & IN_ATTACK | IN_ATTACK2 | IN_ZOOM))
		{
			cmd->iButtons &= ~IN_ATTACK;
			cmd->iButtons &= ~IN_ATTACK2;
			cmd->iButtons &= ~IN_ZOOM;
		}

		if (DEBUG_HOOKS) L::Debug("CM_Start");
		// movmeent fix
		movement->CM_Start(cmd, pSendPacket);

		// movement
		if (DEBUG_HOOKS) L::Debug("Bunnyhop");
		movement->Bunnyhop();

		if (DEBUG_HOOKS) L::Debug("AutoStrafe");
		movement->AutoStrafe();

		// movmeent fix
		if (DEBUG_HOOKS) L::Debug("CM_MoveFixStart");
		movement->CM_MoveFixStart();

		// lol
		if (cfg->aa.Enable)
		{
			G::cmd->angViewAngle.x = 89;
			G::cmd->angViewAngle.y = G::StartAngle.y + 180.f;
			// p100 legit aa
			/*if (!bSendPacket)
				G::cmd->angViewAngle.y += 30.f;*/
		}

		// offensive 
		/*if (DEBUG_HOOKS) L::Debug("aimbot");
		aimbot->Run();*/

		if (DEBUG_HOOKS) L::Debug("backtrack");
		backtrack->Run();

		// movmeent fix
		if (DEBUG_HOOKS) L::Debug("CM_MoveFixEnd");
		movement->CM_MoveFixEnd();

		// movmeent fix
		if (DEBUG_HOOKS) L::Debug("CM_End");
		movement->CM_End();
	}

	return false; //silent aim on false (only for client)
}

void __stdcall H::PaintTraverseHook(int vguiID, bool force, bool allowForcing)
{
	if  (DEBUG_HOOKS) L::Debug("PaintTraverseHook");

	// noscope
	if (strcmp("HudZoom", I::panel->GetName(vguiID)) == 0 && true && G::LocalplayerAlive)
		return;

	oPaintTraverse(I::panel, vguiID, force, allowForcing);
	if (I::panel && strcmp(I::panel->GetName(vguiID), "MatSystemTopPanel") == 0) {

		if (!G::Localplayer || !I::engine->IsInGame())
			return;

		esp->RunPaintTraverse();
	}
}

void __fastcall H::CamToFirstPeronHook()
{
	if  (DEBUG_HOOKS) L::Debug("CamToFirstPeronHook");

	thirdperson->Run_hkCamToFirstPeron();
}

void __stdcall H::DoPostScreenEffectsHook(int param)
{
	if  (DEBUG_HOOKS) L::Debug("DoPostScreenEffectsHook");

	thirdperson->Run_DoPostScreenEffects();

	return oDoPostScreenEffects(I::clientmode, param);
}
