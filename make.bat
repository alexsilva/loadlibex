call ../libconfig.bat

rem *********************************************************************
rem * aqui coloco as CFLAGS que funcionam (não aceitou /O1 )            *
rem *********************************************************************

set CFLAGS=/nologo /MD /I %INC% /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /c

set OUT_FILE=..\loadlib.dll
set DEF_FILE=loadlib.def
set LIB_FILE=..\loadlib.lib

set SRCS=loadlib.c
set OBJS=loadlib.obj

call ../libbuilt.bat
