#include "libstr.c"
#include "libc.h"

int main(int argc, char** argv){
	char* buf = malloc(10);
	char* buf2 = malloc(5);
	for(int i = 5; i < 10; i++){
		buf[i] = 0;
	}
	buf[0] = 'x';
	buf[1] = 'x';
	buf[2] = 'x';
	buf[3] = 'x';
	buf[4] = 'x';

	buf2[0] = 'y';
	buf2[1] = 'y';
	buf2[2] = 'y';
	buf2[3] = 'y';
	buf2[4] = 'y';
	strcat(buf, buf2);
	puts("TEST:\n");
	puts(buf);
	return 0;
}
