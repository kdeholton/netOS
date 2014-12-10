#include "vga.h"
#include "u8250.h"
#include "console.h"
#include "kbd.h"

U8250 *Console::u8250 = nullptr;
Console* Console::me = nullptr;


void Console::init(U8250* u, Console* c){
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

void Console::put(char ch){
	putcolor(ch, VGA::BLACK, VGA::WHITE);
/*	vga->cursor(row, col, VGA::BLACK, VGA::WHITE);
  if(ch == ~0xf) {//Left arrow key
    cursorLeft();
  }
  if(ch == ~0xe) {//Right arrow key
    cursorRight();
  }
  if(ch == ~0xd) {//Up arrow key
    cursorUp();
  }
  if(ch == ~0xc) {//Down arrow key
    cursorDown();
  }
	if(ch == 8 || ch == 0x7f){
		decrementColumn();
		vga->put(row, col, ' ', VGA::BLACK, VGA::WHITE);
	}
	if(ch >= 0x20 && ch <= 0x7e){
		vga->put(row, col, ch, VGA::BLACK, VGA::WHITE);
		incrementColumn();
	}
	if(ch == '\n'){
		//length = 0;
		incrementRow();
	}
	vga->cursor(row, col, VGA::GREEN, VGA::WHITE);*/
}


void Console::putcolor(char ch, int bg, int fg){
	vga->cursor(row, col, bg, fg);
  if(ch == ~0xf) {//Left arrow key
    cursorLeft();
  }
  if(ch == ~0xe) {//Right arrow key
    cursorRight();
  }
  if(ch == ~0xd) {//Up arrow key
    cursorUp();
  }
  if(ch == ~0xc) {//Down arrow key
    cursorDown();
  }
	if(ch == 8 || ch == 0x7f){
		decrementColumn();
		vga->put(row, col, ' ', bg, fg);
	}
	if(ch >= 0x20 && ch <= 0x7e){
		vga->put(row, col, ch, bg, fg);
		incrementColumn();
	}
	if(ch == '\n'){
		//length = 0;
		incrementRow();
	}
	vga->cursor(row, col, bg, fg);
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
	//char ch = u8250->get();
  char ch = Keyboard::is->get();
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

/*void Console::shiftUpBuf(){
	for(int r = 1; r < ROWS; r++){
		for(int c = 0; c < COLS; c++){
			vga->put(r - 1, c, vga->get(r, c), vga->getBg(r, c), vga->getFg(r, c));
		}
	}
	row--;
	clearRow(ROWS - 1);
}*/

void Console::clearRow(int r){
	for(int c = 0; c < COLS; c++){
		vga->put(r, c, ' ', VGA::BLACK, VGA::WHITE);
	}
}

void Console::cursorLeft(){
  if(length == 0) return;
  if(col > 0) col--;
  else decrementRow();
}

void Console::cursorRight(){
  col++;
  if(col >= COLS)
    incrementRow();
}

void Console::cursorUp(){
  row--;
  if(row<0);
    //shiftDown();
}

void Console::cursorDown(){
  row++;
  if(row >= ROWS)
    shiftUp();
}

