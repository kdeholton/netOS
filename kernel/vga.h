#ifndef _VGA_H_
#define _VGA_H_

#include "stdint.h"

#define ROWS 25
#define COLS 80

class VGA {
public:
    static int BLACK;
    static int BLUE;
    static int GREEN;
    static int CYAN;
    static int RED;
    static int MAGENTA;
    static int BROWN;
    static int LIGHT_GRAY;
    static int DARK_GRAY;
    static int LIGHT_BLUE;
    static int LIGHT_GREEN;
    static int LIGHT_CYAN;
    static int LIGHT_RED;
    static int LIGHT_MAGENTA;
    static int LIGHT_BROWN;
    static int WHITE;
    static int oldRow;
    static int oldCol;

    void put(int r, int c, char ch, int bg, int fg);
    char get(int r, int c);
    char getFg(int r, int c);
    char getBg(int r, int c);
    void cursor(int r, int c, int bg, int fg);
};

#endif
