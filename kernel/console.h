#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "vga.h"
#include "io.h"
#include "u8250.h"

class Console : public OutputStream<char>{
	VGA* vga;
	int row;
	int col;
	int length;
	
public:
	static U8250* u8250;
	static Console* me;
	Console() {
		vga = new VGA();
		length = 0;
		//buf = new char[512];
	}
	static void init(U8250* u, Console* c);

	virtual char get();
	virtual void put(char ch);
	virtual void shiftUp();
	virtual void clearRow(int r);
	virtual void incrementColumn();
	virtual void incrementRow();
	virtual void decrementColumn();
	virtual void decrementRow();
  virtual void clear();
};

#endif
