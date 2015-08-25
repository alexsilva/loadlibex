char *loadlib_c = "$Id: loadlib.c,v 1.10 1999/03/23 01:11:12 rborges Exp $";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "loadlib.h"

/*
  define one of these macros to define the dl interface: WIN32, SHL, RLD or DLFCN

  systems that support some kind of dynamic linking:
  WIN32: MS Windows 95/98/NT
  SHL: HP-UX
  RLD: NeXT
  DLFCN: Linux, SunOS, IRIX, UNIX_SV, OSF1, SCO_SV, BSD/OS
  DLFCN (simulation): AIX
*/

#if defined(WIN32)
#define MAP_FORMAT "%s%s.dll"
#else
#define MAP_FORMAT "%slib%s.so"
#endif


#if defined(WIN32)

#include <windows.h>
typedef HINSTANCE libtype;
typedef FARPROC   functype;
#define loadfunc(lib,name) GetProcAddress( lib, name )
#define unloadlibrary(lib) FreeLibrary( lib )
#define liberror()         dll_error("Could not load library.")
#define funcerror()        dll_error("Could not load function.")
 
static libtype loadlibrary( char* path )
{
  libtype libhandle = LoadLibrary( path );
  if (!libhandle)
  {
    int maxtries = 10;
    do 
    {
      maxtries--;
      Sleep( 2 );
      libhandle = LoadLibrary( path );
    } while( !libhandle &&
             maxtries>0 &&
             GetLastError()==ERROR_SHARING_VIOLATION );
  }
  return libhandle;
}
 
#define BUFFER_SIZE 100
 
static char* dll_error( char* altmsg )
{
  static char buffer[BUFFER_SIZE+1];
  if ( FormatMessage( FORMAT_MESSAGE_IGNORE_INSERTS |
                      FORMAT_MESSAGE_FROM_SYSTEM,
                      0, /* source */
                      GetLastError(),
                      0, /* langid */
                      buffer,
                      BUFFER_SIZE,
                      0 /* arguments */ ) )
  {
    return buffer;
  }
  else
  {
    return altmsg;
  }
}

#elif defined(DLFCN)

#include <dlfcn.h>
#ifndef RTLD_GLOBAL
#define RTLD_GLOBAL 0
#endif
typedef void* libtype;
typedef lua_CFunction functype;
#define loadlibrary(path)  dlopen( path, RTLD_LAZY | RTLD_GLOBAL )
#define loadfunc(lib,name) (functype)dlsym( lib, name )
#define unloadlibrary(lib) dlclose( lib )
#define liberror()         dlerror()
#define funcerror()        dlerror()

#elif defined(SHL)

#include <dl.h>
typedef shl_t libtype;
typedef lua_CFunction functype;
#define loadlibrary(path)  shl_load( path, BIND_DEFERRED | BIND_NOSTART, 0L )
#define unloadlibrary(lib) shl_unload( lib )
#define liberror()         "Could not load library."
#define funcerror()        "Could not load function."

static functype loadfunc( libtype lib, char *name )
{
  functype fn;
  if (shl_findsym( &lib, name, TYPE_PROCEDURE, &fn ) == -1)
    return 0;
  return fn;
}

#elif defined(RLD)

#include <rld.h>
typedef long libtype;
typedef lua_CFunction functype;
#define loadlibrary(path)  rldload( 0, 0, path, 0  )
#define unloadlibrary(lib) ;
#define liberror()         "Could not load library."
#define funcerror()        "Could not load function."

static functype loadfunc( libtype lib, char *name )
{
  functype fn;
  char* _name = (char*)malloc((strlen(name)+2)*sizeof(char));
  if (!_name) return 0;
  _name[0] = '_';
  strcpy( _name+1, name );
  if (!rld_lookup( 0, _name, &fn ))
  {
    free(_name);
    return 0;
  }
  free(_name);
  return fn;
}

#else

typedef void *libtype;
typedef lua_CFunction functype;
#define loadlibrary(path)  (0)
#define loadfunc(lib, name) (0)
#define unloadlibrary(lib) ;
#define liberror()         "Dynamic libraries not supported."
#define funcerror()        ""

#endif

#define LIBTAG      1
#define UNLOADEDTAG 2

#define FIRSTARG    3


static int gettag(lua_State *L, int i) {
    return (int) lua_getnumber(L, lua_getparam(L, i));
}


static libtype check_libhandle(lua_State *L, int nparam, lua_Object lh) {
    luaL_arg_check(L, lua_isuserdata(L, lh), nparam, "userdata expected");
    luaL_arg_check(L, lua_tag(L, lh) == gettag(L, LIBTAG), nparam, "not a valid library handle");
    return (libtype) lua_getuserdata(L, lh);
}

static void loadlib(lua_State *L) {
    int tag = gettag(L, LIBTAG);
    char *libname = luaL_check_string(L, FIRSTARG);
    char *path;
    libtype lib;

    if (strpbrk(libname, ".:/\\")) {
        path = libname;
    }
    else {
        lua_Object param = lua_getparam(L, FIRSTARG + 1);
        char *dir = "";
        if (param != LUA_NOOBJECT) {
            dir = luaL_check_string(L, FIRSTARG + 1);
        }

        path = (char *) malloc(sizeof(char) * (strlen(dir) +
                                               strlen(libname) +
                                               strlen(MAP_FORMAT) + 1));
        if (!path) lua_error(L, "not enough memory.");
        sprintf(path, MAP_FORMAT, dir, libname);
    }

    lib = loadlibrary(path);
    if (path != libname) free(path);
    if (!lib) {
        lua_pushnil(L);
        lua_pushstring(L, liberror());
        return;
    }
    lua_pushusertag(L, lib, tag);
}

static void callfromlib(lua_State *L) {
    libtype lh = check_libhandle(L, 1, lua_getparam(L, FIRSTARG));
    char *funcname = luaL_check_string(L, FIRSTARG + 1);
    functype fn = loadfunc(lh, funcname);
    if (fn) {
        fn(L);
    }
    else {
        lua_error(L, funcerror());
    }
}

static void unloadlib(lua_State *L) {
    lua_Object lh = lua_getparam(L, FIRSTARG);unloadlibrary(check_libhandle(L, 1, lh));
    lua_pushobject(L, lh);
    lua_settag(L, gettag(L, UNLOADEDTAG));
}

int LUA_LIBRARY loadlib_open(lua_State *L) {
    static struct luaL_reg funcs[] = {
            {"loadlib",     loadlib},
            {"unloadlib",   unloadlib},
            {"callfromlib", callfromlib}
    };
    int libtag, unloadedtag;
    int i;
    libtag = lua_newtag(L);
    unloadedtag = lua_newtag(L);
    for (i = 0; i < sizeof(funcs) / sizeof(funcs[0]); i++) {
        lua_pushnumber(L, libtag);
        lua_pushnumber(L, unloadedtag);
        lua_pushcclosure(L, funcs[i].func, 2);
        lua_setglobal(L, funcs[i].name);
    }
    lua_pushstring(L, LOADLIB_VERSION);
    lua_setglobal(L, "LOADLIB_VERSION");
    return 0;
}
  
