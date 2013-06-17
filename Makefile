PREFIX?=/usr/X11R6
CFLAGS?=-Os -Wall

all:
	$(CC) $(CFLAGS) -I$(PREFIX)/include tinywm.c -L$(PREFIX)/lib -lX11 -o tinywm

ceu:
	ceu tinywm.ceu
	$(CC) $(CFLAGS) -I$(PREFIX)/include main.c -L$(PREFIX)/lib -lX11 -o tinywm

xcb:
	$(CC) $(CFLAGS) -I$(PREFIX)/include tinywm-xcb.c -L$(PREFIX)/lib -lxcb -lxcb-keysyms -o tinywm

xcb-ceu:
	ceu tinywm-xcb.ceu
	#ceu xxx.ceu
	$(CC) $(CFLAGS) -I$(PREFIX)/include main.c -L$(PREFIX)/lib -lxcb -lxcb-keysyms -o tinywm

clean:
	rm -f tinywm

