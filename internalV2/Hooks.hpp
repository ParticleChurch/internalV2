
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
	typedef void(__thiscall* CamToFirstPeron)(void*);
	typedef void(__thiscall* DoPostScreenEffects)(void*, int);
	typedef bool(__thiscall* WriteUsercmdDeltaToBuffer)(void*, int, void*, int, int, bool);
	typedef void(__thiscall* DrawModelExecute)(void*, void*, void*, const ModelRenderInfo& info, matrix3x4_t* customBoneToWorld);

	// orig func ptrs
	extern Reset oReset;
	extern LockCursor oLockCursor;
	extern EndScene oEndScene;
	extern FrameStageNotify oFrameStageNotify;
	extern CreateMove oCreateMove;
	extern PaintTraverse oPaintTraverse;
	extern CamToFirstPeron oCamToFirstPeron;
	extern DoPostScreenEffects oDoPostScreenEffects;
	extern WriteUsercmdDeltaToBuffer oWriteUsercmdDeltaToBuffer;
	extern DrawModelExecute oDrawModelExecute;

	//our functions
	extern long __stdcall ResetHook(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters);
	extern void __stdcall LockCursorHook();
	extern long __stdcall EndSceneHook(IDirect3DDevice9* device);
	extern LRESULT __stdcall WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	extern void __stdcall FrameStageNotifyHook(int stage);
	extern bool __stdcall CreateMoveHook(float flInputSampleTime, CUserCmd* cmd);
	extern void __stdcall PaintTraverseHook(int vguiID, bool force, bool allowForcing);
	extern void __fastcall CamToFirstPeronHook();
	extern void __stdcall DoPostScreenEffectsHook(int param);
	extern	bool __fastcall WriteUsercmdDeltaToBufferHook(void* ecx, void* edx, int slot, void* buffer, int from, int to, bool isnewcommand);
	extern void __fastcall DrawModelExecuteHook(void* thisptr, int edx, void* ctx, void* state, const ModelRenderInfo& info, matrix3x4_t* customBoneToWorld);
}
