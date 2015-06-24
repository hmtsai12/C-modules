#
# Makefile for the usbtest example package
#

CFLAGS+=$(TARGET_CFLAGS)

LDFLAGS+=-lusb
all: usbtest

usbtest: main.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c 
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f usbtest *.o

