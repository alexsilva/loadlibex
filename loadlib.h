#ifndef loadlib_h
#define loadlib_h

#define LOADLIB_VERSION     "LoadlibEx 2.0-beta"
#define LOADLIB_COPYRIGHT   "Copyright (C) 1996-1999 TeCGraf"
#define LOADLIB_AUTHOR      "R. Borges; Alex"

#if defined(_WIN32) //  Microsoft
#define LUA_LIBRARY __declspec(dllexport)
#else //  Linux
#define LUA_LIBRARY __attribute__((visibility("default")))
#endif

#define LOADLIB_ERROR   1
#define LOADLIB_NOERROR 0
#define LOADLIB_OK      "OK"

struct loadlib_st {
    int code;
    char *msg;
};

int LUA_LIBRARY loadlibs_open(lua_State *L);

#endif

