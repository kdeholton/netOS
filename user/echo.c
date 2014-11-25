#include "libc.h"

int main(int argc, char** argv) {
    char *sep = "";
    if(1>=argc)return 0;
    for (int i=1; i<argc; i++) {
        puts(sep);
        sep = " ";        
        puts(argv[i]);
    }
    puts("\n");
    return 0;
}
