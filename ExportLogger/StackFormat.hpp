#pragma once
// https://github.com/JochenKalmbach/StackWalker
#include "StackWalker/StackWalker.h"

#ifdef PRINT_CALLSTACK
#define CALLSTACK_MSG_BEGIN     "       Call Stack: \n"
#define CALLSTACK_MSG_END       "---------------------------------------------- \n"
#else
#define CALLSTACK_MSG_BEGIN     ""
#define CALLSTACK_MSG_END       ""
#endif

extern char cFileName[50];

class StackFormat : public StackWalker
{
public:
    StackFormat() : StackWalker() {}
protected:
    virtual void OnOutput(LPCSTR szText) {
        FILE* hFile = fopen(cFileName, "a");
        fwrite(szText, strlen(szText), 1, hFile);
        fclose(hFile);
    }
};