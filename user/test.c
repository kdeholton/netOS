#include "libc.h"
#include "libstr.c"

int main(int argc, char** argv) {
	int i = open("f1.txt");
	char* buf2 = malloc(1500);
	for(int k = 0; k < 512; k++){
		buf2[k] = 'x';
	}
	for(int k = 512; k < 999; k++){
		buf2[k] = 'y';
	}
	for(int k = 999; k < 1499; k++){
		buf2[k] = 'z';
	}
	buf2[512] = '\n';
	buf2[1499] = 0;
	write(i, buf2, 1500);
	close(i);
	buf2 = malloc(1500);
	int j = open("f1.txt");
	if(j < 0){
		puts("I have failed, master\n");
	}
	readFully(j, buf2, 1500);
	puts("Testing f1.txt: writing longer than original size\n");
	puts("Length: "); putdec(getlen(j)); puts("\n");
	puts("Length of buf is suppposedly "); putdec(strlen(buf2)); puts("\n");
	//puts(buf2);


	close(j);

	
	
	return 0;
}
