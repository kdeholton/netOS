#include "libc.h"
#include "libstr.c"

int main(int argc, char** argv) {
	puts("Seeing if we correctly add/free blocks\n");
	puts("This test continually writes larger/smaller files to see if they get added to avail correctly\n");
	int id = 0;
	char* buf2;
	for(int i = 0; i < 10000; i++){
		//Write file of two blocks
		id = open("f1.txt");
		buf2 = malloc(700);
		for(int k = 0; k < 700; k++){
			buf2[k] = 'x';
		}
		write(id, buf2, 700);
		close(id);

		id = open("f1.txt");
		readFully(id, buf2, 700);
		if(getlen(id) != 700){
			puts("\nTest2 failed writing size 700 on iteration "); putdec(i); puts("\n");
			return -1;
		}
		close(id);

		free(buf2);

		//Write file of one block
		id = open("f1.txt");
		buf2 = malloc(200);
		for(int k = 0; k < 200; k++){
			buf2[k] = 'y';
		}
		write(id, buf2, 200);
		close(id);

		id = open("f1.txt");
		readFully(id, buf2, 700);
		if(getlen(id) != 200){
			puts("\nTest2 failed writing size 200 on iteration "); putdec(i); puts("\n");
		}
		close(id);

		free(buf2);

		if(i % 1000 == 0){
			putdec(i); puts("...");
		}
	}
	puts("\nCompleted 10000 iterations\n");
	return 0;
}
