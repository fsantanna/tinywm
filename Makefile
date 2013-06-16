PREFIX?=/usr/X11R6
CFLAGS?=-Os -Wall

all:
	$(CC) $(CFLAGS) -I$(PREFIX)/include tinywm.c -L$(PREFIX)/lib -lX11 -o tinywm

ceu:
	ceu tinywm.ceu
	$(CC) $(CFLAGS) -I$(PREFIX)/include main.c -L$(PREFIX)/lib -lX11 -o tinywm
clean:
	rm -f tinywm

