
PREFIX?=  /usr
BINDIR?=  $(PREFIX)/bin
MANDIR?=  $(PREFIX)/share/man/man1

CC?=      cc
CFLAGS+=  -Wall -Wextra -O2 -std=gnu11

PROGRAMS= basename cksum dirname false hostid link logname \
          printenv sync true tty unlink whoami

MANPAGES= $(addsuffix .1,$(PROGRAMS))

.PHONY: all install clean uninstall

all: $(PROGRAMS)

%: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

install: all
	@for program in $(PROGRAMS); do \
		install -Dm 0755 $$program $(DESTDIR)$(BINDIR)/$$program || exit 1; \
		install -Dm 0644 $$program.1 $(DESTDIR)$(MANDIR)/$$program.1 || exit 1; \
	done

clean:
	rm -f $(PROGRAMS)

uninstall:
	@for program in $(PROGRAMS); do \
		rm -f $(DESTDIR)$(BINDIR)/$$program; \
		rm -f $(DESTDIR)$(MANDIR)/$$program.1; \
	done

