#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "vga.h"
#include "io.h"
#include "u8250.h"
#include "semaphore.h"

class Console : public OutputStream<char>{
	VGA* vga;
	int row;
	int col;
	int length;
	
public:
	static U8250* u8250;
	static Console* me;
	static Mutex* mutex;
	Console() {
		vga = new VGA();
		length = 0;
		//buf = new char[512];
	}
	static void init(U8250* u, Console* c);

	virtual char get();
	virtual int put(char ch);
	virtual int putcolor(char ch, int bg, int fg);
	virtual void shiftUp();
	virtual void shiftDown();
	virtual void clearRow(int r);
	virtual void incrementColumn();
	virtual void incrementRow();
	virtual void decrementColumn();
	virtual void decrementRow();
  virtual void clear();
  virtual int cursorLeft();
  virtual int cursorRight();
  virtual int cursorUp();
  virtual int cursorDown();
	virtual void moveToZero();
  virtual int getRow();
  virtual int getColumn();
  virtual void setCursor(int x, int y);
};

#endif
