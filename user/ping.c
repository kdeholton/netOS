#include "libc.h"

int main(int argc, char** argv){
	if(argc <= 1){
		puts("No address was passed\n");
		return 0;
	}
	char* addr = argv[1];
	puts("Pinging: "); puts(addr); puts(" . . . \n");
	int gottenResult = 1;
	char* result = malloc(6);
	result[0] = 'U';
	result[1] = ' ';
	result[2] = 'S';
	result[3] = 'U';
	result[4] = 'K';
	result[5] = 0;
	while(!gottenResult){
		//WAIT FOR RESULT
	}
	puts("Result: "); puts(result); puts("\n");
	return 0;
}
