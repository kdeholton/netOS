#include "vga.h"
#include "u8250.h"
#include "console.h"

U8250 *Console::u8250 = nullptr;
Console* Console::me = nullptr;


void Console::init(U8250* u, Console* c){
	U8250::init(u);
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
}

void Console::put(char ch){
	vga->cursor(row, col, VGA::BLACK, VGA::WHITE);
	if(ch == 8 || ch == 0x7f){
		decrementColumn();
		vga->put(row, col, ' ', VGA::BLACK, VGA::WHITE);
	}
	if(ch >= 0x20 && ch <= 0x7e){
		vga->put(row, col, ch, VGA::BLACK, VGA::WHITE);
		incrementColumn();
	}
	if(ch == '\n'){
		length = 0;
		incrementRow();
	}
	vga->cursor(row, col, VGA::GREEN, VGA::WHITE);
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
	char ch = u8250->get();
	return ch;
	//return u8250->get();
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

void Console::clearRow(int r){
	for(int c = 0; c < COLS; c++){
		vga->put(r, c, ' ', VGA::BLACK, VGA::WHITE);
	}
}
