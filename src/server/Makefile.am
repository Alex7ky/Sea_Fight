bin_PROGRAMS = server

server_SOURCES = server.c
server_SOURCES += gen_ships.c 
server_SOURCES += common.h gen_ships.h server.h

LIBS = $(PTHREAD_LIBS)

AM_CFLAGS = -Wall -I$(top_builddir)/src/header
AM_CFLAGS += $(PTHREAD_CFLAGS)
AM_CFLAGS += -I$(top_builddir)/src/server
AM_CFLAGS += -I$(top_builddir)/src
