#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdio.h>
#include <time.h>

#include "minhook/include/MinHook.h"
#include "StackFormat.hpp"
#include "Hijack.h"

// #define GLOBAL_MODE // record all modules
// #define PRINT_CALLSTACK // WARNING: this cause target executable lag hard, only turn on when you need it

#define TARGET_MODULE "HPSocket4C-SSL.dll"

char cFileName[50] = { 0 };
tm* LogTime;

void __stdcall Log(const char* pFormatted)
{
    FILE* hFile = fopen(cFileName, "a");
    fwrite(pFormatted, strlen(pFormatted), 1, hFile);
    fclose(hFile);
}

void __stdcall LogCall(const char* sExport)
{
    char sLog[255] = { 0 };
    sprintf(sLog, "[*][CALL] %d:%d:%d export: %s\n" CALLSTACK_MSG_BEGIN, LogTime->tm_hour, LogTime->tm_min, LogTime->tm_sec, sExport);
    Log(sLog);
#ifdef PRINT_CALLSTACK
    StackFormat sw; sw.ShowCallstack();
    Log(CALLSTACK_MSG_END);
#endif
}

void __stdcall LogGet(const char* pMsg)
{
    char sLog[255] = { 0 };
    sprintf(sLog, "[*][GET] %d:%d:%d export: %s\n", LogTime->tm_hour, LogTime->tm_min, LogTime->tm_sec, pMsg);
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
    static int iCounter = 1; // why we don't use unordered_map or record return address? because its pointless, there could be wrapper function for calling getprocaddress. better print the stack.
#ifndef GLOBAL_MODE
    static HMODULE pTargetModule;
    if (!pTargetModule)
    {
        pTargetModule = GetModuleHandleA(TARGET_MODULE);
    }
    if(pTargetModule == hModule)
#endif
    {
        char cSequence[255] = { 0 };
        if ((uintptr_t)sExport < 0x1000) // get by order?
            sprintf(cSequence, "Order %d [Seq:%d]", (uintptr_t)sExport, iCounter);
        else 
            sprintf(cSequence, "%s [Seq:%d]", sExport, iCounter);
        iCounter++;
        LogGet(cSequence);
        return (FARPROC)GenerateLoggerStub(pOriginalExport, cSequence);
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
        sprintf(cFileName, "Log_[%d-%d-%d]-[%d-%d-%d].txt", 1900 + LogTime->tm_year, LogTime->tm_mon + 1, LogTime->tm_mday, LogTime->tm_hour, LogTime->tm_min, LogTime->tm_sec);
        Log("==============================================\n");
        Log("ExporLogger made by Extr3lackLiu\n");
        Log("https://github.com/extremeblackliu/ExportLogger\n");
        Log("==============================================\n");
        MH_Initialize();
        MH_CreateHookApi(L"kernel32.dll", "GetProcAddress", hkGetProcAddress, (void**)&oGetProcAddress);
        MH_EnableHook(MH_ALL_HOOKS);
    }
    return TRUE;
}

