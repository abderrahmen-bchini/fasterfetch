UNAME   := $(shell uname -s)
CC      = gcc
CFLAGS  = -O2 -Wall -Wextra
PREFIX  = /usr/local
BINDIR  = $(PREFIX)/bin
DATADIR = $(PREFIX)/share/myfetch
MANDIR  = $(PREFIX)/share/man/man1

.PHONY: all install uninstall clean

all: myfetch

myfetch: myfetch.c
	$(CC) $(CFLAGS) myfetch.c -o myfetch

ifeq ($(UNAME), Darwin)
install: myfetch
	install -d $(DESTDIR)$(BINDIR) $(DESTDIR)$(MANDIR)
	install -m 755 myfetch   $(DESTDIR)$(BINDIR)/myfetch
	install -m 644 myfetch.1 $(DESTDIR)$(MANDIR)/myfetch.1
	@if [ -f ascii.txt ]; then install -d $(DESTDIR)$(DATADIR) && install -m 644 ascii.txt $(DESTDIR)$(DATADIR)/ascii.txt; fi
else
install: myfetch
	install -Dm755 myfetch   $(DESTDIR)$(BINDIR)/myfetch
	install -Dm644 myfetch.1 $(DESTDIR)$(MANDIR)/myfetch.1
	@if [ -f ascii.txt ]; then install -Dm644 ascii.txt $(DESTDIR)$(DATADIR)/ascii.txt; fi
endif

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/myfetch
	rm -f $(DESTDIR)$(DATADIR)/ascii.txt
	rm -f $(DESTDIR)$(MANDIR)/myfetch.1
	-rmdir $(DESTDIR)$(DATADIR) 2>/dev/null || true

clean:
	rm -f myfetch
