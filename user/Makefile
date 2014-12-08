PROGS = shell ls shutdown echo cat ping clear sl edit forkbomb test

all : $(PROGS)

-include ../common.mak

shell : CFILES=shell.c libc.c heap.c

echo : CFILES=echo.c libc.c heap.c

ls : CFILES=ls.c libc.c heap.c

shutdown : CFILES=shutdown.c libc.c heap.c

cat : CFILES=cat.c libc.c heap.c

ping : CFILES=ping.c libc.c heap.c

clear : CFILES=clear.c libc.c heap.c

sl : CFILES=sl.c libc.c heap.c

edit : CFILES=edit.c libc.c heap.c

forkbomb : CFILES=forkbomb.c libc.c heap.c

test : CFILES=test.c libc.c heap.c

$(PROGS) : % : Makefile $(OFILES)
	ld -N -m elf_i386 -e start -Ttext=0x80000000 -o $@ $(OFILES)

clean ::
	rm -f $(PROGS)
