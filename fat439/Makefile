CFLAGS = -std=c99 -m32 -g -O3 -Wall -Werror
PROGS = mkfs

#OFILES = $(subst .c,.o,$(CFILES))
OFILES = $(filter %.o,$^)

all : user.img;

mkfs : mkfs.o

# keep all files
.SECONDARY :


FILES = ../user/shutdown ../user/shutdown.c ../user/shell.c ../user/shell ../user/ls.c ../user/ls ../user/echo ../user/echo.c ../user/cat.c ../user/cat f1.txt f2.txt panic ../user/ping.c ../user/ping ../user/clear.c ../user/clear ../user/sl.c ../user/sl ../user/edit ../user/forkbomb ../user/test.c ../user/test ../user/test2.c ../user/test2 ../user/cmatrix.c ../user/cmatrix ../user/teststr.c ../user/teststr f3.txt

../user/% :
	make -C ../user

user.img : mkfs $(FILES)
	./mkfs user.img 1024 $(FILES)

%.o :  Makefile %.c
	gcc -c -MD $(CFLAGS) $*.c

$(PROGS) : % : Makefile
	gcc $(CFLAGS) -o $@ $(OFILES)

clean ::
	rm -f $(PROGS) *.img
	rm -f *.o
	rm -f *.d

-include *.d
