#include "libc.h"
#include "libstr.c"

void notFound(char* cmd) {
    puts(cmd);
    puts(": command not found\n");
}

long isELF(int argc, char** argv){
	long id = open(argv[0]);
	if(id<0)return id;
	else{
		long len = getlen(id);
		if(len < 4)return 0;
		char* buf = (char*)malloc(5);
		readFully(id, (void*)buf, 4);
		buf[4] = 0;
		if(buf[0] == 0x7f
				&& buf[1] == 'E'
				&& buf[2] == 'L'
				&& buf[3] == 'F'){
			long id = fork();
			if(id == 0){ // CHILD
				execv(argc, argv);
				exit(-1);
			}
			else if(id < 0){ //ERROR
			}
			else{ //PARENT
				join(id);
			}
			return 0;
		}
		return 1;
	}
}

int main() {
puts("                  x    x              x      xxxxxxx    xxxxx\n");
puts("                  xx   x    xxxx      x      x     x   x\n");     
puts("                  x x  x   x    x  xxxxxxx   x     x    xx\n");
puts("                  x  x x   xxxxxx     x      x     x      xx\n");
puts("                  x   xx   x          x      x     x        x\n");
puts("                  x    x    xxxx      x      xxxxxxx   xxxxx\n");
puts("\n\n\n\n\n\n\n\n\n\n");
    while (1) {
        puts("shell> ");
        char* in = gets();
	int* argc = malloc(4);
	char** argv = tokenize(in, argc);
	if(argv == 0){
		continue;
	}
	//Echo command
	/*if(strEquals(argv[0], "echo") == 0 || strEquals(argv[0], "ls") == 0
			||strEquals(argv[0], "cat") == 0
			||strEquals(argv[0], "shutdown") == 0){
		long id = fork();
		if(id == 0){ // CHILD
			execv(*argc, argv);
			exit(-1);
		}
		else if(id < 0){ //ERROR
		}
		else{ //PARENT
			join(id);
			continue;
		}
	}*/
	int ret = isELF(*argc, argv);
	if(ret < 0)
		notFound(*argv);
	else if(ret == 0) continue;
	else{
		char** newptr = malloc(12);
		char*  str    = malloc(4);
		str[0] = 'c';
		str[1] = 'a';
		str[2] = 't';
		str[3] = '\0';
		newptr[0] = str;
		newptr[1] = argv[0];
		newptr[2] = 0;
		long id = fork();
		if(id == 0){ // CHILD
			execv(2, newptr);
			exit(-1);
		}
		else if(id < 0){ //ERROR
		}
		else{ //PARENT
			join(id);
		}
	}
	if (in) free(in);
    }
    return 0;
}
