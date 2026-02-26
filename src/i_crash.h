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
//   Platform-specific crash handling interface.
//   Provides abstraction for Windows and Unix-like systems.
//

#ifndef __I_CRASH__
#define __I_CRASH__

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void I_WindowsStackTrace(void *ExceptionInfo);
long I_HandleException(void *ExceptionInfo);

#endif

#endif
