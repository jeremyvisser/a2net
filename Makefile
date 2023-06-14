CC=cl65
LD=cl65

CFLAGS+=-t apple2enh -C apple2enh-system.cfg
LDFLAGS+=-t apple2enh -C apple2enh-system.cfg

# By default, CC65 includes an "AppleSingle" header, suitable for use with
# AppleCommander (java -jar ac.jar -as my.dsk < mybinary). But if you are
# transferring the binary directly to the Apple, use this:
LDFLAGS+=--ld-args -D__EXEHDR__=0

all: a2net serdbg

clean:
	rm -f a2net *.o *.map

a2net: a2net.o log.o util.o comms.o slip.o packet.o ip.o icmp.o

serdbg: serdbg.o

.PHONY: all clean

