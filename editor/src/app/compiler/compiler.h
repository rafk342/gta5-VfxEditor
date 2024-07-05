//
// File: compiler.h
//
// Copyright (C) 2023 ranstar74. All rights violated.
//
// Part of "Rage Am" Research Project.
//
#pragma once

#define AM_NORET		__declspec(noreturn)
#define AM_NOINLINE		__declspec(noinline)
#define AM_FORCEINLINE	__forceinline

#define am_version		1
#define with_console	0
#define test_ver		0
#define Using_DrawList	0

#define gameVer3095	1
#define gameVer2060	2

#define game_version gameVer3095

#if am_version
#define AM_EXPORT extern "C" __declspec(dllexport)
#else
#define AM_EXPORT
#endif

