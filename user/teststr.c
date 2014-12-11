#include "libstr.c"
#include "libc.h"

int main(int argc, char** argv){
	char* buf = malloc(10);
	for(int i = 0; i < 10; i++){
		buf[i] = i + 65;
	}

	puts("BEFORE: \n");
	puts(buf);
	puts("\n");
	char* test = deleteChar(buf, 4);
	puts("AFTER:\n");
	puts(test);
	puts("\n");
	return 0;
}
