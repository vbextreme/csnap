#Easy Makefile

APPPRJNAME = snap
LIBPRJNAME = libcsnap.so
HMASTER    = csnap.h
LIBSO      = 
LIBA       =
APPIPATH   = /usr/bin/
LIBIPATH   = /usr/lib/
IHPATH     = /usr/include/
APPCFLAGS  = -Wall -g 
APPDFLAGS  = -Wall -g
LIBCFLAGS  = -Wall -O3
LIBDFLAGS  = -O3
APPDEFINE  = -D_APP
LIBDEFINE  = -D_LIB

APPCC      = gcc
APPLK      = gcc
LIBCC      = gcc
LIBLK      = ld
CFILE      = $(shell echo *.c)
HFILE      = $(shell echo *.h)
APPOBJPATH = ./obj/
LIBOBJPATH = ./oobj/
APPOFILE   = $(patsubst %.c,$(APPOBJPATH)%.o,$(CFILE)) 
LIBOFILE   = $(patsubst %.c,$(LIBOBJPATH)%.o,$(CFILE)) 

all:
	@echo use app or lib

app: $(APPPRJNAME)
	@echo end make $(APPPRJNAME)

$(APPPRJNAME): $(APPOFILE)
	$(APPLK) $(APPDFLAGS) -o $(APPPRJNAME) $(APPOFILE) $(LIBA) $(LIBSO)

$(APPOBJPATH)%.o: %.c $(HFILE)
	$(APPCC) $(APPCFLAGS) -c $< -o $@ $(APPDEFINE)

lib: $(LIBPRJNAME)
	@echo end make $(LIBPRJNAME)

$(LIBPRJNAME): $(LIBOFILE)
	$(LIBLK) $(LIBDFLAGS) -shared -o $(LIBPRJNAME) $(LIBOFILE) $(LIBA) $(LIBSO)

$(LIBOBJPATH)%.o: %.c $(HFILE)
	$(LIBCC) $(LIBCFLAGS) -fpic -c $< -o $@ $(LIBDEFINE)

appclean:
	rm -rf $(APPOBJPATH)*.o
	rm -rf ./$(APPPRJNAME)

libclean:
	rm -rf $(LIBOBJPATH)*.o
	rm -rf ./$(LIBPRJNAME)

appinstall:
	cp ./$(APPPRJNAME) $(APPIPATH)$(APPPRJNAME)

libinstall:
	cp ./$(LIBPRJNAME) $(LIBIPATH)$(LIBPRJNAME)
	cp ./$(HMASTER) $(IHPATH)$(HMASTER)
