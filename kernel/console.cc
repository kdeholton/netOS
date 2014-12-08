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
  this->row = 0;
  this->col = 0;
}

void Console::editBorder(){
  this->row = 21;
  this->col = 0;
  int i;
  for(i=0; i<80; i++){
    vga->put(21,i,'*',VGA::BLACK, VGA::GREEN);
  }

  //Exit & Save
  vga->put(22,10,'^',VGA::WHITE, VGA::BLACK);
  vga->put(22,11,'X',VGA::WHITE, VGA::BLACK);
  vga->put(22,12,' ',VGA::BLACK, VGA::WHITE);
  vga->put(22,13,'E',VGA::BLACK, VGA::WHITE);
  vga->put(22,14,'X',VGA::BLACK, VGA::WHITE);
  vga->put(22,15,'I',VGA::BLACK, VGA::WHITE);
  vga->put(22,16,'T',VGA::BLACK, VGA::WHITE);
  vga->put(22,17,' ',VGA::BLACK, VGA::WHITE);
  vga->put(22,18,'&',VGA::BLACK, VGA::WHITE);
  vga->put(22,19,' ',VGA::BLACK, VGA::WHITE);
  vga->put(22,20,'S',VGA::BLACK, VGA::WHITE);
  vga->put(22,21,'A',VGA::BLACK, VGA::WHITE);
  vga->put(22,22,'V',VGA::BLACK, VGA::WHITE);
  vga->put(22,23,'E',VGA::BLACK, VGA::WHITE);


  //Exit & Discard
  vga->put(23,10,'^',VGA::WHITE, VGA::BLACK);
  vga->put(23,11,'Z',VGA::WHITE, VGA::BLACK);
  vga->put(23,12,' ',VGA::BLACK, VGA::WHITE);
  vga->put(23,13,'E',VGA::BLACK, VGA::WHITE);
  vga->put(23,14,'X',VGA::BLACK, VGA::WHITE);
  vga->put(23,15,'I',VGA::BLACK, VGA::WHITE);
  vga->put(23,16,'T',VGA::BLACK, VGA::WHITE);
  vga->put(23,17,' ',VGA::BLACK, VGA::WHITE);
  vga->put(23,18,'&',VGA::BLACK, VGA::WHITE);
  vga->put(23,19,' ',VGA::BLACK, VGA::WHITE);
  vga->put(23,20,'D',VGA::BLACK, VGA::WHITE);
  vga->put(23,21,'I',VGA::BLACK, VGA::WHITE);
  vga->put(23,22,'S',VGA::BLACK, VGA::WHITE);
  vga->put(23,23,'C',VGA::BLACK, VGA::WHITE);
  vga->put(23,24,'A',VGA::BLACK, VGA::WHITE);
  vga->put(23,25,'R',VGA::BLACK, VGA::WHITE);
  vga->put(23,26,'D',VGA::BLACK, VGA::WHITE);


  //Save
  vga->put(22,32,'^',VGA::WHITE, VGA::BLACK);
  vga->put(22,33,'W',VGA::WHITE, VGA::BLACK);
  vga->put(22,34,' ',VGA::BLACK, VGA::WHITE);
  vga->put(22,35,'S',VGA::BLACK, VGA::WHITE);
  vga->put(22,36,'A',VGA::BLACK, VGA::WHITE);
  vga->put(22,37,'V',VGA::BLACK, VGA::WHITE);
  vga->put(22,38,'E',VGA::BLACK, VGA::WHITE);

  vga->cursor(0,0,VGA::GREEN,VGA::WHITE);
  this->row = 0;
  this->col = 0;
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
