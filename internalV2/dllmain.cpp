#include "Include.hpp"


HMODULE dllModule;

void onInject()
{
    // seed random
    srand(static_cast <unsigned> (time(0)));

    // initalizers
    L::init();

    // TODO: wait for them to press eject
    L::Info("Hello World!");

    // uninitalizers
    L::free();

    // deallocate DLL
    FreeLibraryAndExitThread(dllModule, 0);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReasonForCall, LPVOID lpReserved)
{
    switch (dwReasonForCall)
    {
    case DLL_PROCESS_ATTACH:
        dllModule = hModule;
        CreateThread(0, 0, LPTHREAD_START_ROUTINE(onInject), 0, 0, 0);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }

    return true;
}

