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

class EventListener : public GameEventListener
{
public:
	EventListener()
	{
		I::gameeventmanager->AddListener(this, "bullet_impact");	// Resolver
		I::gameeventmanager->AddListener(this, "player_hurt");		// Resolver/Hitmarker
		I::gameeventmanager->AddListener(this, "weapon_fire");		// Resolver
		I::gameeventmanager->AddListener(this, "player_death");		// Hitmarker
		I::gameeventmanager->AddListener(this, "round_freeze_end");	// AA (freezetime)
		I::gameeventmanager->AddListener(this, "round_prestart");	// AA (freezetime)
	}
	~EventListener()
	{
		I::gameeventmanager->RemoveListener(this);
	}

	virtual void FireGameEvent(GameEvent* event)
	{
		switch (StrHash::HashRuntime(event->GetName())) {
		case StrHash::Hash("bullet_impact"):
		{
			int UserID = event->GetInt("userid");

			if (I::engine->GetPlayerForUserID(UserID) != G::LocalplayerIndex)
				return;

			Vector loc = Vector(event->GetFloat("x"), event->GetFloat("y"), event->GetFloat("z"));
			resolver->Impact(loc);
		}
		break;
		case StrHash::Hash("weapon_fire"):
		{
			/*
			Name:	weapon_fire
			Structure:	
			short	userid	
			string	weapon	weapon name used
			bool	silenced	is weapon silenced
			*/
			int UserID = event->GetInt("userid");

			if (I::engine->GetPlayerForUserID(UserID) != G::LocalplayerIndex)
				return;

			resolver->Shot();
		}
		break;
		case StrHash::Hash("player_hurt"):
		{
			/*
			player_hurt
			Name:	player_hurt
			Structure:
			short	userid	user ID of who was hurt
			short	attacker	user ID of who attacked
			byte	health	remaining health points
			byte	armor	remaining armor points
			string	weapon	weapon name attacker used, if not the world
			short	dmg_health	damage done to health
			byte	dmg_armor	damage done to armor
			byte	hitgroup	hitgroup that was damaged
			*/
			// if the localplayer gets hurt, return
			int UserID = event->GetInt("userid");
			if (I::engine->GetPlayerForUserID(UserID) == G::LocalplayerIndex)
			{
				/*if(event->GetInt("dmg_health") < 100)
					I::engine->ExecuteClientCmd("kill");*/
				return;
			}

			// if the localplayer isn't shootin, return
			int iAttacker = event->GetInt("attacker");
			if (I::engine->GetPlayerForUserID(iAttacker) != G::LocalplayerIndex)
				return;

			resolver->Hurt(event->GetInt("userid"), event->GetInt("hitgroup"), event->GetInt("dmg_health"));

		}
		break;
		case StrHash::Hash("player_death"):
		{
			/*
			player_death
			Note: When a client dies

			Name:	player_death
			Structure:
			short	userid	user ID who died
			short	attacker	user ID who killed
			short	assister	user ID who assisted in the kill
			bool	assistedflash	assister helped with a flash
			string	weapon	weapon name killer used
			string	weapon_itemid	inventory item id of weapon killer used
			string	weapon_fauxitemid	faux item id of weapon killer used
			string	weapon_originalowner_xuid
			bool	headshot	singals a headshot
			short	dominated	did killer dominate victim with this kill
			short	revenge	did killer get revenge on victim with this kill
			short	wipe	To do: check if indicates on a squad wipeout in Danger Zone
			short	penetrated	number of objects shot penetrated before killing target
			bool	noreplay	if replay data is unavailable, this will be present and set to false
			bool	noscope	kill happened without a scope, used for death notice icon
			bool	thrusmoke	hitscan weapon went through smoke grenade
			bool	attackerblind	attacker was blind from flashbang
			*/


		}
		break;
		case StrHash::Hash("round_freeze_end"):

			break;
		case StrHash::Hash("round_prestart"):

			break;
		}
	}
};

namespace H
{
	// VMT Managers
	VMT::Manager* d3d9VMT = nullptr;
	VMT::Manager* surfaceVMT = nullptr;
	VMT::Manager* clientVMT = nullptr;
	VMT::Manager* clientmodeVMT = nullptr;
	VMT::Manager* panelVMT = nullptr;
	VMT::Manager* inputVMT = nullptr;
	VMT::Manager* modelrenderVMT = nullptr;

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
	WriteUsercmdDeltaToBuffer oWriteUsercmdDeltaToBuffer;
	DrawModelExecute oDrawModelExecute;
	
	// special GUI Vars
	bool D3dInit = false;
	HWND CSGOWindow = NULL;
	std::vector<std::string> console;

	// special vars
	void* WriteUsercmdDeltaToBufferReturn;
	uintptr_t WriteUsercmd;
	int TicksToShift = 0;
	int TicksToRecharge = 0;
	EventListener* g_EventListener;
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
	WriteUsercmdDeltaToBufferReturn = *(reinterpret_cast<void**>(FindPattern("engine.dll", "84 C0 74 04 B0 01 EB 02 32 C0 8B FE 46 3B F3 7E C9 84 C0 0F 84")));
	WriteUsercmd = FindPattern("client.dll", " 55 8B EC 83 E4 F8 51 53 56 8B D9 8B 0D");

	// do VMTs
	L::Debug("-->VMT's");
	d3d9VMT = new VMT::Manager((VMT::VMT*)D3d9Device);
	surfaceVMT = new VMT::Manager((VMT::VMT*)I::surface);
	clientVMT = new  VMT::Manager((VMT::VMT*)I::client);
	clientmodeVMT = new VMT::Manager((VMT::VMT*)I::clientmode);
	panelVMT = new VMT::Manager((VMT::VMT*)I::panel);
	inputVMT = new VMT::Manager((VMT::VMT*)I::input);
	modelrenderVMT = new VMT::Manager((VMT::VMT*)I::modelrender);

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
	L::Debug("-->WriteUsercmdDeltaToBuffer");
	oWriteUsercmdDeltaToBuffer = (WriteUsercmdDeltaToBuffer)clientVMT->Hook(24, &WriteUsercmdDeltaToBufferHook);
	L::Debug("-->DrawModelExecute");
	oDrawModelExecute = (DrawModelExecute)modelrenderVMT->Hook(21, &DrawModelExecuteHook);

	// special lol
	g_EventListener = new EventListener();

	ConsoleColorMsg(Color(0, 255, 0, 255), "Hooks hooked :D\n");
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
	if (modelrenderVMT) modelrenderVMT->UnhookAll();

	// do some last minute adjustments
	if (G::pSendpacket) *G::pSendpacket = true;

	L::Debug("Freeing VMTs");

	// free the VMTs now that they are useless
	if (d3d9VMT) delete d3d9VMT;
	if (surfaceVMT) delete surfaceVMT;
	if (clientVMT) delete clientVMT;
	if (clientmodeVMT) delete clientmodeVMT;
	if (panelVMT) delete panelVMT;
	if (inputVMT) delete inputVMT;
	if (modelrenderVMT) delete modelrenderVMT;

	static auto brightness = I::convar->FindVar("mat_force_tonemap_scale");
	brightness->SetValue(1.f);

	// special detour shit
	L::Debug("Freeing Hacks");

	// now delete hacks
	delete autowall;
	delete movement;
	delete backtrack;
	delete lagcomp;
	delete esp;
	delete rage;
	delete thirdperson;
	delete zeusbot;
	delete dlightManager;
	delete g_EventListener;
	delete chams;

	// let user do input lol
	I::inputsystem->EnableInput(true);

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

	static void* firstReturn = _ReturnAddress();

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
	if (firstReturn != _ReturnAddress())
	{
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if (menu->ConsoleWindow)
		{
			ImGui::SetNextWindowSizeConstraints(ImVec2(400, 400), ImVec2(1920, 1080));
			ImGui::Begin("Console");
			//ImGui::Text(("Aimbot Scans (per tick): " + std::to_string(aimbot->TheoreticalScans)).c_str());
			for (auto& a : H::console)
				ImGui::Text(a.c_str());
			ImGui::End();
		}

		menu->Render();
		esp->RunEndScene();


		/*aimbot->RenderStats();
		lagcomp->RenderStats();*/
		
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

//sigs
#define sig_player_by_index "85 C9 7E 2A A1"
#define sig_draw_server_hitboxes "55 8B EC 81 EC ? ? ? ? 53 56 8B 35 ? ? ? ? 8B D9 57 8B CE"
void __stdcall H::FrameStageNotifyHook(int stage)
{
	if  (DEBUG_HOOKS) L::Debug("FrameStageNotifyHook");

	G::IsInGame = I::engine->IsInGame();

	static bool* disablePostProcessing = *reinterpret_cast<bool**>(FindPattern("client.dll", "83 EC 4C 80 3D") + 5);
	if (stage == FRAME_RENDER_START || stage == FRAME_RENDER_END)
	{
		*disablePostProcessing = stage == FRAME_RENDER_START;
	}

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
		{
			// if no aa...
			if(!cfg->aa.Enable && !cfg->aa.LegitAA)
				*(QAngle*)((DWORD)G::Localplayer + deadflagOffset + 4) = G::EndAngle;
			// if aa
			else
				*(QAngle*)((DWORD)G::Localplayer + deadflagOffset + 4) = QAngle(G::real.x, G::real.y);
		}
	}

	/*
	if (stage == FRAME_RENDER_START && G::Localplayer && G::LocalplayerAlive && cfg->Keybinds["Thirdperson"].boolean)
	{
		static auto util_playerbyindex = FindPattern("server.dll", "85 C9 7E 32 A1"); 
		static auto draw_server_hitboxes = FindPattern("server.dll", "55 8B EC 81 EC ? ? ? ? 53 56 8B 35 ? ? ? ? 8B D9 57 8B CE");

		auto t = -1.0f;

		auto idx = I::engine->GetLocalPlayer();

		L::Debug("asm bitches");

		__asm {
			mov ecx, idx
			call util_playerbyindex
			cmp eax, 0
			je gay
			pushad
			movss xmm1, t
			push 0
			mov ecx, eax
			call draw_server_hitboxes
			popad
		}

	gay:
		{

		}
	}
	*/

	oFrameStageNotify(stage);

	resolver->Run();

	lagcomp->Run_FSN(stage);
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

	//Anti exploiting, will need to deal with later lol
	/*static CConVar* cl_lagcompensation = I::convar->FindVar("cl_lagcompensation");
	cl_lagcompensation->SetValue(0);*/

	if (I::engine->IsInGame() && cmd && G::Localplayer)
	{
		PVOID pebp;
		__asm mov pebp, ebp;
		bool* pSendPacket = (bool*)(*(DWORD*)pebp - 0x1C);
		bool& bSendPacket = *pSendPacket;

		// Menu fix
		if (G::MenuOpen && (cmd->iButtons & IN_ATTACK | IN_ATTACK2 | IN_ZOOM))
		{
			cmd->iButtons &= ~IN_ATTACK;
			cmd->iButtons &= ~IN_ATTACK2;
			cmd->iButtons &= ~IN_ZOOM;
		}

		// fps fix
		static auto shadows = I::convar->FindVar("cl_csm_enabled");
		shadows->SetValue(false);

		// movmeent fix
		movement->CM_Start(cmd, pSendPacket);

		// aa movement
		antiaim->RunMovement();

		// rank reveal
		if ((G::cmd->iButtons & IN_SCORE))
			I::client->DispatchUserMessage(50, 0, 0, nullptr);
		
		// movement
		movement->Bunnyhop();

		movement->AutoStrafe();

		// movement
		movement->SlowWalk();

		// movmeent fix
		movement->CM_MoveFixStart();

		// fakelag
		antiaim->FakelagStart();
		// put stuff like exploit start here (for predicted and actual value of sendpacket)
		if (TicksToShift > 0 || TicksToRecharge)
			antiaim->PredictedVal = true;

		bSendPacket = antiaim->FakelagEnd();

		// run antiaim aa
		antiaim->Run();

		// run fakeduck
		if (cfg->Keybinds["Fake Duck"].boolean)
		{
			G::cmd->iButtons |= IN_BULLRUSH;
			if (I::engine->GetNetChannelInfo()->chokedPackets > 7) // usually 7
				G::cmd->iButtons |= IN_DUCK;
			else
				G::cmd->iButtons &= ~IN_DUCK;
		}
		

		// offensive 
		backtrack->Run();

		// Manual Shot fix
		if (G::cmd->iButtons & IN_ATTACK && G::Localplayer->CanShoot())
		{
			G::cmd->angViewAngle = G::StartAngle;
			G::real = Vector(G::cmd->angViewAngle.x, G::cmd->angViewAngle.y);
			G::fake = Vector(G::cmd->angViewAngle.x, G::cmd->angViewAngle.y);
		}

		// E fix
		if (G::cmd->iButtons & IN_USE)
		{
			G::cmd->angViewAngle = G::StartAngle;
			G::real = Vector(G::cmd->angViewAngle.x, G::cmd->angViewAngle.y);
			G::fake = Vector(G::cmd->angViewAngle.x, G::cmd->angViewAngle.y);
			for (auto& a : lagcomp->PlayerList)
				a.second.badShots++;
		}

		rage->Run();

		// Shot fix
		static bool LastTickShot = false;
		if (LastTickShot)
		{
			G::real = Vector(G::cmd->angViewAngle.x, G::cmd->angViewAngle.y);
			G::fake = Vector(G::cmd->angViewAngle.x, G::cmd->angViewAngle.y);
			LastTickShot = false;
		}
		if (G::cmd->iButtons & IN_ATTACK)
		{
			LastTickShot = true;
			G::real = Vector(G::cmd->angViewAngle.x, G::cmd->angViewAngle.y);
			G::fake = Vector(G::cmd->angViewAngle.x, G::cmd->angViewAngle.y);
		}

		// movmeent fix
		movement->CM_MoveFixEnd();

		// movmeent fix
		movement->CM_End();

		if ((G::cmd->iButtons & IN_ATTACK) && G::Localplayer->CanShoot())
		{
			if (cfg->aimbot.EnableHs)
				TicksToShift = 3;
			if (cfg->aimbot.EnableDt)
				TicksToShift = 16;
			bSendPacket = true;
		}
		
		// recharge dt (but if we NEED to shoot, and can... shoot)
		static int LastChargeTickcount = G::cmd->iTickCount;
		if (TicksToRecharge > 0 && !(G::cmd->iButtons & IN_ATTACK) && G::Localplayer->CanShoot())
		{
			if (G::cmd->iTickCount != LastChargeTickcount)
			{
				TicksToRecharge-= 1; // dummy thicc maths
				LastChargeTickcount = G::cmd->iTickCount;
			}
			G::cmd->iTickCount = INT_MAX;
		}
		
		// dummy aimbot fix?
		if (TicksToShift > 0)
			G::cmd->iButtons |= IN_ATTACK;

		

	}

	return false; //silent aim on false (only for client)
}

void __stdcall H::PaintTraverseHook(int vguiID, bool force, bool allowForcing)
{
	/*if  (DEBUG_HOOKS) L::Debug("PaintTraverseHook");*/

	// noscope
	if (strcmp("HudZoom", I::panel->GetName(vguiID)) == 0 && cfg->vis.NoScope && G::LocalplayerAlive)
		return;

	oPaintTraverse(I::panel, vguiID, force, allowForcing);
	if (I::panel && strcmp(I::panel->GetName(vguiID), "MatSystemTopPanel") == 0) {
		static DWORD FONT = I::surface->FontCreate();
		static bool Once = true;
		if (Once)
		{
			Once = false;
			I::surface->SetFontGlyphSet(FONT, "Verdana", 16, 1, 0, 0, FONTFLAG_DROPSHADOW);// FONTFLAG_ANTIALIAS | FONTFLAG_OUTLINE);
		}

		static float framerate;
		framerate = 0.9f * framerate + (1.0f - 0.9f) * I::globalvars->flAbsFrameTime;
		if (framerate <= 0.0f)
			framerate = 1.0f;

		std::string TEXT = "FPS: " + std::to_string((int)(1.f / framerate));
		static std::wstring wide_string;
		wide_string = std::wstring(TEXT.begin(), TEXT.end());

		I::surface->DrawSetTextFont(FONT);
		I::surface->DrawSetTextColor(Color(0, 255, 0, 255));
		I::surface->DrawSetTextPos(10, 10);
		I::surface->DrawPrintText(wide_string.c_str(), wcslen(wide_string.c_str()));

		if (!G::Localplayer || !I::engine->IsInGame())
			return;

		/*if (DEBUG_HOOKS) L::Debug("manager start");
		dlightManager->Run();
		if (DEBUG_HOOKS) L::Debug("manager end");*/


		esp->RunPaintTraverse();

		antiaim->Visualize();

		// if player isn't scoped, or the value isnt checked, dont draw it
		if (!G::Localplayer->IsScoped() || !cfg->vis.NoScope)
			return;

		int xSize;
		int ySize;
		I::engine->GetScreenSize(xSize, ySize);
		I::surface->DrawSetColor(Color(0, 0, 0, 255));
		I::surface->DrawLine(xSize / 2, ySize / 4, xSize / 2, ySize * 3 / 4); //Top - Bottom
		I::surface->DrawLine(xSize / 4, ySize / 2, xSize * 3 / 4, ySize / 2); //Left - Right

		Vector origin = G::Localplayer->GetVecOrigin();
		Vector nextPos = Calc::ExtrapolateTick(origin, G::Localplayer->GetVecVelocity(), 10);
		Vector screen1, screen2;
		if (WorldToScreen(nextPos, screen1) && WorldToScreen(origin, screen2))
		{
			I::surface->DrawSetColor(Color(255,255,255,255));
			I::surface->DrawLine(screen1.x, screen1.y, screen2.x, screen2.y);
		}

		/*if (G::Localplayer->ScopeLevel() > 0)
		{
			int width, height;
			I::engine->GetScreenSize(width, height);

			I::surface->DrawSetColor(Color(0, 0, 0, 255));
			I::surface->DrawLine(width / 2, 0, width / 2, height);
			I::surface->DrawLine(0, height / 2, width, height / 2);
		}*/
		

		

	}
}

void __fastcall H::CamToFirstPeronHook()
{
	if  (DEBUG_HOOKS) L::Debug("CamToFirstPeronHook");

	thirdperson->Run_hkCamToFirstPeron();
	/*oCamToFirstPeron(I::input);*/
}

void __stdcall H::DoPostScreenEffectsHook(int param)
{
	if  (DEBUG_HOOKS) L::Debug("DoPostScreenEffectsHook");

	thirdperson->Run_DoPostScreenEffects();
	if (I::engine->IsInGame())
	{
		static auto brightness = I::convar->FindVar("mat_force_tonemap_scale");
		brightness->SetValue(cfg->world_vis.brightness);
	}

	return oDoPostScreenEffects(I::clientmode, param);
}

static void CWriteUsercmd(void* buf, CUserCmd* Cin, CUserCmd* Cout)
{
	static auto WriteUsercmdF = H::WriteUsercmd;

	__asm
	{
		mov ecx, buf
		mov edx, Cin
		push Cout
		call WriteUsercmdF
		add esp, 4
	}
}

bool __fastcall H::WriteUsercmdDeltaToBufferHook(void* ecx, void* edx, int slot, void* buf, int from, int to, bool isnewcommand)
{
	//return oWriteUsercmdDeltaToBuffer(ecx, slot, buf, from, to, isnewcommand);
	static auto ofunct = oWriteUsercmdDeltaToBuffer;

	// if no shift...
	if (TicksToShift <= 0)
		return ofunct(ecx, slot, buf, from, to, isnewcommand);

	if (from != -1)
		return true;

	auto CL_SendMove = []() {
		using CL_SendMove_t = void(__fastcall*)(void);
		static CL_SendMove_t CL_SendMoveF = (CL_SendMove_t)FindPattern("engine.dll", "55 8B EC A1 ? ? ? ? 81 EC ? ? ? ? B9 ? ? ? ? 53 8B 98");

		CL_SendMoveF();
	};

	int* pNumBackupCommands = (int*)(reinterpret_cast<uintptr_t>(buf) - 0x30);
	int* pNumNewCommands = (int*)(reinterpret_cast<uintptr_t>(buf) - 0x2C);
	auto net_channel = *reinterpret_cast<INetChannelInfo**>(reinterpret_cast<uintptr_t>(I::clientstate) + 0x9C);
	int32_t new_commands = *pNumNewCommands;

	int32_t next_cmdnr = I::clientstate->iLastOutgoingCommand + I::clientstate->nChokedCommands + 1;
	int32_t total_new_commands = TicksToShift;//min(doubletap->m_tick_to_shift, 16);
	TicksToShift -= total_new_commands;
	TicksToRecharge += total_new_commands;

	from = -1;
	*pNumNewCommands = total_new_commands;
	*pNumBackupCommands = 0;

	for (to = next_cmdnr - new_commands + 1; to <= next_cmdnr; to++) {
		if (!ofunct(ecx, slot, buf, from, to, isnewcommand))
			return false;

		from = to;
	}

	CUserCmd* last_realCmd = I::input->GetUserCmd(slot, from);
	CUserCmd fromCmd;

	if (last_realCmd)
		fromCmd = *last_realCmd;

	CUserCmd toCmd = fromCmd;
	toCmd.iCommandNumber++;
	toCmd.iTickCount++;

	for (int i = new_commands; i <= total_new_commands; i++) {
		CWriteUsercmd(buf, &toCmd, &fromCmd);
		fromCmd = toCmd;
		toCmd.iCommandNumber++;
		toCmd.iTickCount++;
	}
	return true;
}

void __fastcall H::DrawModelExecuteHook(void* thisptr, int edx, void* ctx, void* state, const ModelRenderInfo& info, matrix3x4_t* customBoneToWorld)
{
	chams->Init(); // BRO, there MUST be a better way to do this
	chams->Run(thisptr, edx, ctx, state, info, customBoneToWorld);
}
