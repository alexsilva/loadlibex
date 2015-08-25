#ifndef loadlib_h
#define loadlib_h

#define LOADLIB_VERSION     "LoadlibEx 1.0-beta"
#define LOADLIB_COPYRIGHT   "Copyright (C) 1996-1999 TeCGraf"
#define LOADLIB_AUTHOR      "R. Borges" 

#if defined(_WIN32) //  Microsoft
#define LUA_LIBRARY __declspec(dllexport)
#else //  Linux
#define LUA_LIBRARY __attribute__((visibility("default")))
#endif

int LUA_LIBRARY loadlib_open(lua_State *L);

#endif

