#include "Include.hpp"
void onInject()
{
    // seed random
    srand(static_cast <unsigned> (time(0)));

    // initalizers
    L::Init();
    I::Init();
    N::Init();
    H::Init();

    while (!G::KillDLL) Sleep(100);

    // uninitalizers
    H::Free();
    L::Free();
    
    // deallocate DLL
    FreeLibraryAndExitThread(G::DLLModule, 0);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReasonForCall, LPVOID lpReserved)
{
    switch (dwReasonForCall)
    {
    case DLL_PROCESS_ATTACH:
        G::DLLModule = hModule;
        CreateThread(0, 0, LPTHREAD_START_ROUTINE(onInject), 0, 0, 0);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }

    return true;
}

