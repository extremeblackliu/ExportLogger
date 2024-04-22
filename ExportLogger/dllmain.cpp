#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdio.h>
#include <time.h>

#include "minhook/include/MinHook.h"
#include "Hijack.h"


#define GLOBAL_MODE 0
#define TARGET_MODULE "HPSocket4C-SSL.dll"

tm* LogTime;

void __stdcall Log(const char* pFormatted)
{
    char cFileName[50] = { 0 };
    sprintf(cFileName, "Log_%d_%d_%d_%d_%d_%d.txt", LogTime->tm_year, LogTime->tm_mon, LogTime->tm_hour, LogTime->tm_hour, LogTime->tm_min, LogTime->tm_sec);
    FILE* hFile = fopen(cFileName, "a");
    
    fwrite(pFormatted, strlen(pFormatted), 1, hFile);
    fclose(hFile);
}

void __stdcall LogCall(const char* sExport)
{
    time_t t = time(0) + 8 * 3600;
    tm* ft = localtime(&t);

    char sLog[255] = { 0 };
    sprintf(sLog, "[*] %d:%d:%d calling export: %s\n", ft->tm_hour, ft->tm_min, ft->tm_sec, sExport);

    return Log(sLog);
}

void __stdcall LogGet(const char* pMsg)
{
    time_t t = time(0);
    tm* ft = localtime(&t);

    char sLog[255] = { 0 };
    sprintf(sLog, "[*] %d:%d:%d getting export: %s\n", ft->tm_hour, ft->tm_min, ft->tm_sec, pMsg);

    return Log(sLog);
}

void* GenerateLoggerStub(FARPROC pOrigin, const char* sExport)
{
    uintptr_t pStub = (uintptr_t)VirtualAlloc(0x0, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    unsigned char pShell[] = { 0x60, 0x68, 0x00, 0x00, 0x00, 0x00, 0xB8, 0xD0, 0x0B, 0xF4, 0x75, 0xFF, 0xD0, 0x61, 0xFF, 0x25, 0x00, 0x00, 0x00, 0x00 };

    uintptr_t pOriginPlace = pStub + sizeof(pShell);
    *reinterpret_cast<uintptr_t*>(pOriginPlace) = (uintptr_t)pOrigin;
    *reinterpret_cast<uintptr_t*>(pShell + 16) = (uintptr_t)pOriginPlace;

    uintptr_t pExportNamePlace = pStub + sizeof(pShell) + sizeof(uintptr_t);
    memcpy((void*)pExportNamePlace, sExport, strlen(sExport));
    *reinterpret_cast<uintptr_t*>(pShell + 2) = (uintptr_t)pExportNamePlace;

    *reinterpret_cast<uintptr_t*>(pShell + 7) = (uintptr_t)LogCall;

    memcpy((void*)pStub, pShell, sizeof(pShell));
    return (void*)pStub;
}

using fnGetProcAddress = FARPROC(__stdcall*)(HMODULE, const char*);
fnGetProcAddress oGetProcAddress;

FARPROC __stdcall hkGetProcAddress(HMODULE hModule, const char* sExport)
{
    FARPROC pOriginalExport = oGetProcAddress(hModule, sExport);
    LogGet(sExport);
#if GLOBAL_MODE == 0
    static HMODULE pTargetModule;
    if (!pTargetModule)
    {
        pTargetModule = GetModuleHandleA(TARGET_MODULE);
    }
    if(pTargetModule == hModule)
#endif
    {
        return (FARPROC)GenerateLoggerStub(pOriginalExport, sExport);
    }
    return pOriginalExport;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        sneakyevil_DllHijack::Initialize();
        time_t t = time(NULL);
        LogTime = localtime(&t);
        
        Log("ExporLogger made by Extr3lackLiu\n");
        
        MH_Initialize();
        MH_CreateHookApi(L"kernel32.dll", "GetProcAddress", hkGetProcAddress, (void**)&oGetProcAddress);
        MH_EnableHook(MH_ALL_HOOKS);
    }
    return TRUE;
}

