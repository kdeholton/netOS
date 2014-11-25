#include "libc.h"
#include "libstr.c"

int main(int argc, char** argv) {
	if(argc < 1) return 0;
	long id = open(".");
	long len = getlen(id);
	char* buf = (char*)malloc(len);
	readFully(id, (void*)buf, len);
	char* name = malloc(13);
	int i;
	for(i=0; i<len; i+=16){
		int j;
		for(j=0; j<12; j++){
			name[j] = buf[i+j];
		}
		name[12] = 0;
		puts(name);
		puts("\n");
	}
	close(id);
	return 0;
}
