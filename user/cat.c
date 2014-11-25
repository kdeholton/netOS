#include "libc.h"

int main(int argc, char** argv) {
	if(argc <= 1)return 0;
	for(int i=1; i<argc; i++){
		long id = open(argv[i]);
		if(id < 0){
			puts(argv[0]);
			puts(": ");
			puts(argv[i]);
			puts(": No such file or directory\n");
		}
		long len = getlen(id);
		char* buf = (char*)malloc(len + 1);
		readFully(id, (void*)buf, len);
		buf[len] = 0;
		puts(buf);
	}
	return 0;
}
