#include "Include.hpp"

#pragma comment(lib, "Winmm.lib")  // for playsounda
//for windprc hook
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace H
{
	// VMT Managers
	VMT::Manager* d3d9VMT = nullptr;
	VMT::Manager* surfaceVMT = nullptr;

	// Original Functions
	Reset oReset;
	LockCursor oLockCursor;
	EndScene oEndScene;
	WNDPROC oWndProc = NULL;
	
	// special GUI Vars
	bool D3dInit = false;
	HWND CSGOWindow = NULL;
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

	// do hooks
	L::Debug("-->oReset");
	oReset = (Reset)d3d9VMT->Hook(16, &ResetHook);
	L::Debug("-->LockCursorHook");
	oLockCursor = (LockCursor)surfaceVMT->Hook(67, &LockCursorHook);
	L::Debug("-->EndSceneHook");
	oEndScene = (EndScene)d3d9VMT->Hook(42, &EndSceneHook);
	L::Debug("-->WndProc");
	oWndProc = WNDPROC(SetWindowLongPtrW(CSGOWindow, GWLP_WNDPROC, LONG_PTR(WndProc)));
}

void H::Free()
{
	// restore the VMTs to normal
	if (d3d9VMT) d3d9VMT->UnhookAll();
	if (surfaceVMT) surfaceVMT->UnhookAll();

	// free the VMTs now that they are useless
	if (d3d9VMT) delete d3d9VMT;
	if (surfaceVMT) delete surfaceVMT;
}

long __stdcall H::ResetHook(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	ImGui_ImplDX9_CreateDeviceObjects();

	auto ogOutput = oReset(device, pPresentationParameters);
	return ogOutput;
}

void __stdcall H::LockCursorHook()
{
	if (G::KillDLL)
		return oLockCursor(I::surface);

	if (G::MenuOpen) {
		return I::surface->UnLockCursor();
	}

	return oLockCursor(I::surface);
}

long __stdcall H::EndSceneHook(IDirect3DDevice9* device)
{
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

		ImGui::Begin("sample menu");
		static bool examplebool = false;
		ImGui::Checkbox("example bool", &examplebool);

		bool open_popup = ImGui::Button("Eject");
		if (open_popup)
		{
			ImGui::OpenPopup("Eject popup");
		}
		if (ImGui::BeginPopup("Eject popup"))
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0, 0, 1.00f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.f, 0.1f, 0.1f, 1.00f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.f, 0.2f, 0.2f, 1.00f));

			ImGui::Text("Are you sure you want to Eject?");
			if (ImGui::Button("Eject##Eject2"))
				G::KillDLL = true;
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("NOTE:\nEjecting can potentially crash your game.\nEjecting can is not complete so if you want to inject another cheat, restart the game.");

			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();

			ImGui::EndPopup();
		}	 
		
		ImGui::End();

		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	}

	return H::oEndScene(device);
}

LRESULT __stdcall H::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
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
