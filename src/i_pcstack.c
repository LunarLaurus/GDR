//
// Copyright(C) 2024 Goblin Dice Rollaz
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//   Windows platform stack trace and crash handling.
//

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dbghelp.h>
#include <stdio.h>

#include "i_crash.h"

// I_WindowsStackTrace - Generates and prints a stack trace on Windows using DBGHELP.DLL
// Uses StackWalk64 to walk the call stack and SymGetLineFromAddr64 for source file/line info.
// Requires symbols (PDB) to be available for meaningful output.
void I_WindowsStackTrace(EXCEPTION_POINTERS *ExceptionInfo)
{
    HANDLE process = GetCurrentProcess();
    HANDLE thread = GetCurrentThread();
    STACKFRAME64 stackframe;
    IMAGEHLP_LINE64 lineInfo;
    DWORD machineType;
    BOOL result;

    memset(&stackframe, 0, sizeof(stackframe));
    memset(&lineInfo, 0, sizeof(lineInfo));
    lineInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

    SymInitialize(process, NULL, TRUE);

    machineType = IMAGE_FILE_MACHINE_I386;
    stackframe.AddrPC.Offset = ExceptionInfo->ContextRecord->Eip;
    stackframe.AddrPC.Mode = AddrModeFlat;
    stackframe.AddrFrame.Offset = ExceptionInfo->ContextRecord->Ebp;
    stackframe.AddrFrame.Mode = AddrModeFlat;
    stackframe.AddrStack.Offset = ExceptionInfo->ContextRecord->Esp;
    stackframe.AddrStack.Mode = AddrModeFlat;

    fprintf(stderr, "\nStack trace:\n");

    while (1)
    {
        result = StackWalk64(machineType, process, thread, &stackframe,
                           ExceptionInfo->ContextRecord, NULL, SymFunctionTableAccess64,
                           SymGetModuleBase64, NULL);

        if (!result)
            break;

        fprintf(stderr, "  0x%08lx", (unsigned long)stackframe.AddrPC.Offset);

        if (SymGetLineFromAddr64(process, stackframe.AddrPC.Offset, NULL, &lineInfo))
        {
            fprintf(stderr, " in %s (%s:%lu)",
                   lineInfo.FunctionName ? lineInfo.FunctionName : "?",
                   lineInfo.FileName, (unsigned long)lineInfo.LineNumber);
        }
        fprintf(stderr, "\n");
    }

    SymCleanup(process);
}

// I_HandleException - Windows unhandled exception filter callback
// Called by the OS when an unhandled exception occurs.
// Prints exception code and instruction pointer, then generates stack trace.
// Returns EXCEPTION_EXECUTE_HANDLER to allow cleanup.
long __stdcall I_HandleException(EXCEPTION_POINTERS *ExceptionInfo)
{
    fprintf(stderr, "\nCRASH: Unhandled exception code 0x%lx at 0x%lx\n",
            ExceptionInfo->ExceptionRecord->ExceptionCode,
            (unsigned long)ExceptionInfo->ContextRecord->Eip);

    I_WindowsStackTrace(ExceptionInfo);

    return EXCEPTION_EXECUTE_HANDLER;
}

#endif
