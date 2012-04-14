#include "common.h"

// DllMain - Initialize and cleanup WiX custom action utils.
extern "C" BOOL WINAPI DllMain(
    __in HINSTANCE hInstance,
    __in ULONG ulReason,
    __in LPVOID lpReserved
    )
{
    UNREFERENCED_PARAMETER(lpReserved);

    switch(ulReason)
    {
    case DLL_PROCESS_ATTACH:
        ::DisableThreadLibraryCalls(hInstance);
        WcaGlobalInitialize(hInstance);
        break;

    case DLL_PROCESS_DETACH:
        WcaGlobalFinalize();
        break;
    }

    return TRUE;
}
