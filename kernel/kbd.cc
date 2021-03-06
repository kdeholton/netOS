#include "kbd.h"
#include "machine.h"
#include "bb.h"
#include "debug.h"

int shift = 0;
int control = 0;
int direction = 0;

InputStream<char> *Keyboard::is;
BB<char> *bb;

void Keyboard::init() {
  bb = new BB<char>(1000);
  is = bb;
}

static char kbd_get(void);

/* interrupt handler */
void Keyboard::handler(void) {
  char ch = kbd_get();
  if(ch != ~0) bb->put(ch); //Discard all upkeystrokes
}

/* internal functions */
static char ul(char x) {
  if (shift) {
    switch(x) {
      case 'a': return 'A';
      case 'b': return 'B';
      case 'c': return 'C';
      case 'd': return 'D';
      case 'e': return 'E';
      case 'f': return 'F';
      case 'g': return 'G';
      case 'h': return 'H';
      case 'i': return 'I';
      case 'j': return 'J';
      case 'k': return 'K';
      case 'l': return 'L';
      case 'm': return 'M';
      case 'n': return 'N';
      case 'o': return 'O';
      case 'p': return 'P';
      case 'q': return 'Q';
      case 'r': return 'R';
      case 's': return 'S';
      case 't': return 'T';
      case 'u': return 'U';
      case 'v': return 'V';
      case 'w': return 'W';
      case 'x': return 'X';
      case 'y': return 'Y';
      case 'z': return 'Z';
      case '`': return '~';
      case '1': return '!';
      case '2': return '@';
      case '3': return '#';
      case '4': return '$';
      case '5': return '%';
      case '6': return '^';
      case '7': return '&';
      case '8': return '*';
      case '9': return '(';
      case '0': return ')';
      case '-': return '_';
      case '=': return '+';
      case '[': return '{';
      case ']': return '}';
      case '\\': return '|';
      case ';': return ':';
      case '\'': return '\"';
      case ',': return '<';
      case '.': return '>';
      case '/': return '?';

      default: return x;
    }
  }
  return x;
}

/* get the next character from the hardware */
static char kbd_get(void) {
  while ((inb(0x64) & 1) == 0);
  uint8_t b = inb(0x60);
  //Debug::printf("%x\n",b);
  //return 0;

  if(b == 0xe0){ //E0 indicates the next code is to do with cursor movement.
    direction = !direction;
    return 255;
  }

  if(control){ //Control key has been depressed.
  }

  if(direction){ //Some key pushed related to moving cursor.
    switch(b) {
      case 0x4b : return 0xf0;//Left arrow
      case 0x4d : return 0xf1;//Right arrow
      case 0x48 : return 0xf2;//Up arrow
      case 0x50 : return 0xf3;//Down arrow
    }
  }

  if(control){ //Some control key has been pushed
    switch(b) {
      case 0x10 : return 0xfe; //^q
      case 0x11 : return 0xfd; //^w
    }
  }

  switch (b) {
    case 0x02 ... 0x0a : return(ul('0' + b - 1));
    case 0x0b : return(ul('0'));
    case 0x0c : return(ul('-'));
    case 0x0d : return(ul('='));
    case 0x0e : return(8); //Backspace
    case 0x10 : return(ul('q'));
    case 0x11 : return(ul('w'));
    case 0x12 : return(ul('e'));
    case 0x13 : return(ul('r'));
    case 0x14 : return(ul('t'));
    case 0x15 : return(ul('y'));
    case 0x16 : return(ul('u'));
    case 0x17 : return(ul('i'));
    case 0x18 : return(ul('o'));
    case 0x19 : return(ul('p'));
    case 0x1a : return(ul('['));
    case 0x1b : return(ul(']'));
    case 0x2b : return(ul('\\'));
    case 0x1c : return(13);
    case 0x1e : return(ul('a'));
    case 0x1f : return(ul('s'));
    case 0x20 : return(ul('d'));
    case 0x21 : return(ul('f'));
    case 0x22 : return(ul('g'));
    case 0x23 : return(ul('h'));
    case 0x24 : return(ul('j'));
    case 0x25 : return(ul('k'));
    case 0x26 : return(ul('l'));
    case 0x27 : return(ul(';'));
    case 0x28 : return(ul('\''));
    case 0x29 : return(ul('`'));
    case 0x2c : return(ul('z'));
    case 0x2d : return(ul('x'));
    case 0x2e : return(ul('c'));
    case 0x2f : return(ul('v'));
    case 0x30 : return(ul('b'));
    case 0x31 : return(ul('n'));
    case 0x32 : return(ul('m'));
    case 0x33 : return(ul(','));
    case 0x34 : return(ul('.'));
    case 0x35 : return(ul('/'));
    case 0x39 : return (' ');


    case 0x1d : control = 1; return 255;
    case 0x9d : control = 0; return 255;


    case 0x2a: case 0x36: shift = 1; return 255;
    case 0xaa: case 0xb6: shift = 0; return 255;
    default: return 255;
  }

}

