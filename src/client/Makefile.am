bin_PROGRAMS = client

client_SOURCES = client.c
client_SOURCES += graphics.c
client_SOURCES += common.h client.h graphic.h client.h

client_LDADD = @CURSES_LIB@

AM_CFLAGS = -Wall -I$(top_builddir)/src/header
AM_CFLAGS += -I$(top_builddir)/src/client
AM_CFLAGS += -I$(top_builddir)/src
