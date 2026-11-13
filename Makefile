UNAME   := $(shell uname -s)
CC      = gcc
CFLAGS  = -O2 -Wall -Wextra -Isrc/include
PREFIX  = /usr/local
BINDIR  = $(PREFIX)/bin
DATADIR = $(PREFIX)/share/fasterfetch
MANDIR  = $(PREFIX)/share/man/man1

SRCS    = src/main.c src/config.c src/utils/utils.c src/utils/json.c src/modules/themes.c \
          src/modules/border.c src/modules/module_registry.c src/modules/logos.c src/modules/sysinfo.c
OBJS    = $(SRCS:.c=.o)

.PHONY: all build install install-user uninstall uninstall-user clean check-perms

all: fasterfetch
build: fasterfetch

fasterfetch: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o fasterfetch

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Catches the common "forgot sudo, forgot PREFIX" case with a clear message
# instead of a cryptic install(1) permission error. Only fires for the
# default system-wide PREFIX; install-user below sidesteps it entirely.
check-perms:
	@if [ "$$(id -u)" != "0" ] && [ -z "$(DESTDIR)" ] && [ -d "$(PREFIX)" ] && [ ! -w "$(PREFIX)" ]; then \
		echo "Error: no permission to write to $(PREFIX)."; \
		echo ""; \
		echo "Either run with sudo:"; \
		echo "  sudo make install"; \
		echo ""; \
		echo "...or install to your home directory without sudo:"; \
		echo "  make install-user"; \
		exit 1; \
	fi

# No sudo, ever -- installs into ~/.local instead of /usr/local.
# Make sure ~/.local/bin is on your PATH.
install-user: fasterfetch
	$(MAKE) install PREFIX=$(HOME)/.local

ifeq ($(UNAME), Darwin)
install: fasterfetch check-perms
	install -d $(DESTDIR)$(BINDIR) $(DESTDIR)$(MANDIR)
	install -m 755 fasterfetch   $(DESTDIR)$(BINDIR)/fasterfetch
	install -m 644 fasterfetch.1 $(DESTDIR)$(MANDIR)/fasterfetch.1
	@if [ -f ascii.txt ]; then install -d $(DESTDIR)$(DATADIR) && install -m 644 ascii.txt $(DESTDIR)$(DATADIR)/ascii.txt; fi
else
install: fasterfetch check-perms
	install -Dm755 fasterfetch   $(DESTDIR)$(BINDIR)/fasterfetch
	install -Dm644 fasterfetch.1 $(DESTDIR)$(MANDIR)/fasterfetch.1
	@if [ -f ascii.txt ]; then install -Dm644 ascii.txt $(DESTDIR)$(DATADIR)/ascii.txt; fi
endif

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/fasterfetch
	rm -f $(DESTDIR)$(DATADIR)/ascii.txt
	rm -f $(DESTDIR)$(MANDIR)/fasterfetch.1
	-rmdir $(DESTDIR)$(DATADIR) 2>/dev/null || true

# Matches install-user -- removes from ~/.local instead of /usr/local.
uninstall-user:
	$(MAKE) uninstall PREFIX=$(HOME)/.local

clean:
	rm -f fasterfetch $(OBJS)
