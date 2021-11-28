
namespace H
{
	// idk why we would need a gui init/free :/
	extern void Init();
	extern void Free();
	extern std::vector < std::string> console;

	//typedef
	typedef long(__stdcall* Reset)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
	typedef void(__thiscall* LockCursor)(void*);
	typedef long(__stdcall* EndScene)(IDirect3DDevice9* device);
	typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
	typedef void(__stdcall* FrameStageNotify)(int);
	typedef bool(__stdcall* CreateMove)(float, CUserCmd*);
	typedef void(__thiscall* PaintTraverse)(void*, int, bool, bool);

	extern Reset oReset;
	extern LockCursor oLockCursor;
	extern EndScene oEndScene;
	extern FrameStageNotify oFrameStageNotify;
	extern CreateMove oCreateMove;
	extern PaintTraverse oPaintTraverse;

	//our functions
	extern long __stdcall ResetHook(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters);
	extern void __stdcall LockCursorHook();
	extern long __stdcall EndSceneHook(IDirect3DDevice9* device);
	extern LRESULT __stdcall WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	extern void __stdcall FrameStageNotifyHook(int stage);
	extern bool __stdcall CreateMoveHook(float flInputSampleTime, CUserCmd* cmd);
	extern void __stdcall PaintTraverseHook(int vguiID, bool force, bool allowForcing);
}
