CC=gcc
CFLAGS=-O2 -DDLFCN -I../luang/include $(DLFCNINC)
OBJS = loadlib.o 

LIB = libloadlib.a

main:	
	@if [ `uname` = AIX ] ; then \
	  (DLFCNINC="-I../dlfcn"; export DLFCNINC ; $(MAKE) $(LIB)) \
	else \
	  ($(MAKE) $(LIB)) \
	fi
 

$(LIB): $(OBJS)
	ar rcu $@ loadlib.o
	ranlib $@

