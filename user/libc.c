#include "libc.h"
#include "sys.h"

static char hexDigits[] = { '0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

void puts(char* p) {
  char c;
  while ((c = *p++) != 0) putchar(c); 
}

char* gets() {
  long sz = 0;
  char *p = 0;
  long i = 0;

  while (1) {
    if (i >= sz) {
      sz += 10;
      p = realloc(p,sz+1);
      if (p == 0) return 0;
    }
    char c = getchar();
    if(c == ~0xf){ //Left Arrow == 0xf0
      //puts("L");
      putchar(~0xf);
      continue;
    }
    if(c == ~0xe){ //Right Arrow == 0xf1
      //puts("R");
      putchar(~0xe);
      continue;
    }
    if(c == ~0xd){ //Up Arrow == 0xf2
      //puts("U");
      putchar(~0xd);
      continue;
    }
    if(c == ~0xc){ //Down Arrow == 0xf3
      //puts("D");
      putchar(~0xc);
      continue;
    }
    if(c == ~0x1){ //^q == 0xfe
      puts("[^q]");
      continue;
    }
    if(c == ~0x2){ //^w == 0xfd
      puts("[^w]");
      continue;
    }
    if(c == 8 || c == 0x7f){
      //puts(p);
      if(i > 0){
        putchar(c);
        p[i--] = 0;
      }
      continue;
    }
    putchar(c);
    if (c == 13) {
      puts("\n");
      p[i] = 0;
      return p;
    }
    p[i++] = c;        
  }
}

void puthex(long v) {
  for (int i=0; i<sizeof(long)*2; i++) {
    char c = hexDigits[(v & 0xf0000000) >> 28];
    putchar(c);
    v = v << 4;
  }
}

void putdec(unsigned long v) {
  if (v >= 10) {
    putdec(v / 10);
  }
  putchar(hexDigits[v % 10]);
}

long readFully(long fd, void* buf, long length) {
  long togo = length;
  char* p = (char*) buf;
  while(togo) {
    long cnt = read(fd,p,togo);
    if (cnt < 0) return cnt;
    if (cnt == 0) return length - togo;
    p += cnt;
    togo -= cnt;
  }
  return length;
}
