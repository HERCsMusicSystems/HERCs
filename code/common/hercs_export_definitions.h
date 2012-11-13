///////////////////////////////////////
// Copyright (C) 2007 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include "operating_system.h"

#ifdef LINUX_OPERATING_SYSTEM
#ifndef HERCs_SDK
#define HERCs_SDK_EXPORT
#else
#define HERCs_SDK_EXPORT
#endif
#endif

#ifdef WINDOWS_OPERATING_SYSTEM
#ifndef HERCs_SDK
#ifdef NO_DLL_EXPORT
#define HERCs_SDK_EXPORT
#else
#define HERCs_SDK_EXPORT __declspec (dllexport)
#endif
#else
#ifdef NO_DLL_EXPORT
#define HERCs_SDK_EXPORT
#else
#define HERCs_SDK_EXPORT __declspec (dllimport)
#endif
#endif
#endif
