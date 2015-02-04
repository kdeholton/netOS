#include "vga.h"
#include "u8250.h"
#include "console.h"
#include "kbd.h"

#define NEWCOL(R, C) (C+1 == COLS ? R+1 : R)
#define NEWROW(R, C) (C+1 == COLS ? 0 : C+1)

U8250 *Console::u8250 = nullptr;
Console* Console::me = nullptr;
Mutex* Console::mutex = nullptr;


void Console::init(U8250* u, Console* c){
	mutex = new Mutex();
	U8250::init(u);
  Keyboard::init();
	u8250 = u;
	me = c;
	for(int i = 0; i < ROWS; i++){
		me->clearRow(i);
	}
	me->clearRow(0);
}

void Console::clear(){
  for(int i = 0; i < ROWS; i++){
    this->clearRow(i);
  }
  this->clearRow(0);
  this->row = 0;
  this->col = 0;
  vga->cursor(0,0,VGA::BLACK,VGA::WHITE);
}

void Console::moveToZero(){
	shiftDown();
	this->row = 0;
	this->col = 0;
	vga->cursor(0, 0, VGA::BLACK, VGA::GREEN);
}

int Console::put(char ch){
	return putcolor(ch, VGA::BLACK, VGA::WHITE);
}


int Console::putcolor(char ch, int bg, int fg){
	int retVal = 0;
	vga->cursor(row, col, bg, fg);
	if(ch == ~0xf) {//Left arrow key
		cursorLeft();
	}
	if(ch == ~0xe) {//Right arrow key
		cursorRight();
	}
	if(ch == ~0xd) {//Up arrow key
		return cursorUp();
	}
	if(ch == ~0xc) {//Down arrow key
		return cursorDown();
	}
	if(ch == 8 || ch == 0x7f){ // backspace
		decrementColumn();
		vga->put(row, col, ' ', bg, fg);
		for(int i = col; i < COLS-1; i++){
			vga->put(row, i, vga->get(row, i+1), bg, fg);
		}
		vga->put(row, COLS-1, ' ', bg, fg);
	}
	if(ch >= 0x20 && ch <= 0x7e){
		vga->put(row, col, ch, bg, fg);
		incrementColumn();
		
	}
	if(ch == '\n'){
		incrementRow();
	}
	vga->cursor(row, col, bg, fg);
	return retVal;
}

void Console::decrementColumn(){
	if(length == 0){
		return;
	}
	length--;
	if(col > 0){
		col--;
	}
	else{
		decrementRow();
	}
}

void Console::decrementRow(){
	row--;
	col = COLS - 1;
}

void Console::decrementOnlyRow(){
	row--;
}

void Console::incrementColumn(){
	length++;
	col++;
	if(col >= COLS){
		incrementRow();
	}
}

void Console::incrementRow(){ // Sets col to 0
	col = 0;
	row++;
	if(row >= ROWS){
		shiftUp();
	}
}

char Console::get(){
	mutex->lock();
  char ch = Keyboard::is->get();
	mutex->unlock();
	return ch;
}

void Console::shiftUp(){
	for(int r = 1; r < ROWS; r++){
		for(int c = 0; c < COLS; c++){
			vga->put(r - 1, c, vga->get(r, c), vga->getBg(r, c), vga->getFg(r, c));
		}
	}
	row--;
	clearRow(ROWS - 1);
}

void Console::shiftDown(){
	for(int r = ROWS - 2; r >= 0; r--){
		for(int c = 0; c < COLS; c++){
			vga->put(r + 1, c, vga->get(r, c), vga->getBg(r, c), vga->getFg(r, c));
		}
	}
	row++;
	clearRow(0);
}


void Console::clearRow(int r){
	for(int c = 0; c < COLS; c++){
		vga->put(r, c, ' ', VGA::BLACK, VGA::WHITE);
	}
}

int Console::cursorLeft(){
  if(length == 0) return 0;
  if(col > 0) col--;
  else decrementRow();
  return 0;
}

int Console::cursorRight(){
  col++;
  if(col >= COLS)
    incrementRow();
  return 0;
}

int Console::cursorUp(){
  row--;
  if(row<0){
    row++;
    vga->cursor(row,col,VGA::BLACK,VGA::WHITE);
    return 1;
  }
  vga->cursor(row,col,VGA::BLACK,VGA::WHITE);
  return 0;
}

int Console::cursorDown(){
  row++;
  if(row >= ROWS){
    row--;
    vga->cursor(row,col,VGA::BLACK,VGA::WHITE);
    return 2;
  }
  vga->cursor(row,col,VGA::BLACK,VGA::WHITE);
  return 0;
}

int Console::getRow(){
  return row;
}

int Console::getColumn(){
  return col;
}

void Console::setCursor(int x, int y){
  row = x;
  col = y;
  vga->cursor(x,y,VGA::GREEN,VGA::WHITE);
  return;
}
