UNAME   := $(shell uname -s)
CC      = gcc
CFLAGS  = -O2 -Wall -Wextra -Isrc/include
PREFIX  = /usr/local
BINDIR  = $(PREFIX)/bin
DATADIR = $(PREFIX)/share/fasterfetch
MANDIR  = $(PREFIX)/share/man/man1

SRCS    = src/main.c src/utils/utils.c src/modules/themes.c src/modules/logos.c src/modules/sysinfo.c
OBJS    = $(SRCS:.c=.o)

.PHONY: all install uninstall clean

all: fasterfetch

fasterfetch: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o fasterfetch

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

ifeq ($(UNAME), Darwin)
install: fasterfetch
	install -d $(DESTDIR)$(BINDIR) $(DESTDIR)$(MANDIR)
	install -m 755 fasterfetch   $(DESTDIR)$(BINDIR)/fasterfetch
	install -m 644 fasterfetch.1 $(DESTDIR)$(MANDIR)/fasterfetch.1
	@if [ -f ascii.txt ]; then install -d $(DESTDIR)$(DATADIR) && install -m 644 ascii.txt $(DESTDIR)$(DATADIR)/ascii.txt; fi
else
install: fasterfetch
	install -Dm755 fasterfetch   $(DESTDIR)$(BINDIR)/fasterfetch
	install -Dm644 fasterfetch.1 $(DESTDIR)$(MANDIR)/fasterfetch.1
	@if [ -f ascii.txt ]; then install -Dm644 ascii.txt $(DESTDIR)$(DATADIR)/ascii.txt; fi
endif

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/fasterfetch
	rm -f $(DESTDIR)$(DATADIR)/ascii.txt
	rm -f $(DESTDIR)$(MANDIR)/fasterfetch.1
	-rmdir $(DESTDIR)$(DATADIR) 2>/dev/null || true

clean:
	rm -f fasterfetch $(OBJS)
