#include "libc.h"
#include "libstr.c"

int numberOfLines(char* line){
  return (strlen(line)+1)/80 + 1;
}

struct listNode {
  char* line;
  struct listNode* next;
  struct listNode* prev;
};

struct listNode* currentTopLine;
int offset;

void display(struct listNode* currentTopLine){
  clear();
  int linesPut = 0;
  struct listNode* currentLine = currentTopLine;
  while(linesPut <= 25){
    if(currentLine == 0 || currentLine->line == 0)
      break;
    if(25 - linesPut < numberOfLines(currentLine->line))
      break;
    if(linesPut != 0)
      puts("\n");
    linesPut += numberOfLines(currentLine->line);
    puts(currentLine->line);
    currentLine = currentLine->next;
  }

}

int onNonprintedLine(struct listNode* currentTopLine, struct listNode* myLine){
  int linesPut = 0;
  struct listNode* currentLine = currentTopLine;
  while(linesPut <= 25){
    if(currentLine == 0 || currentLine->line == 0)
      return 0;
    if(25 - linesPut < numberOfLines(currentLine->line))
      if(currentLine->line == myLine->line)
        return 1;
    linesPut += numberOfLines(currentLine->line);
    currentLine = currentLine->next;
  }
  return 0;
}

struct listNode* currentLine;

void printHead(struct listNode* head){
	struct listNode* current = head;
	while(current != 0){
		if(current->line == 0){
			puts("YOU DONE F***ED UP NOW\n");
		}
		puts(current->line); puts("\n");
		current = current->next;
	}
}

int writeFile(int fd, struct listNode* head){
//  puts("HEAD IS CURRENTLY:\n");
//  printHead(head);
//  puts("\n");
//  puts("Writing!\n");
//  puts("fd : "); putdec(fd); puts("\n");
//  puts("head : "); puthex((long)head); puts("\n");
  int size = 0;

  //Tally up the size
  struct listNode* currentLine;
  currentLine = head;
  while(1){
    if(currentLine == 0 || currentLine->line == 0)
      break;

    size += strlen(currentLine->line);
    size += 1; //Add a char for the '\n' that goes between lines;
    
    currentLine = currentLine->next;
  }
  puts("size:");
  putdec(size);
  puts("\n");
  currentLine = head;
  //printHead(currentLine);
  //Create a buffer of correct size
  char* buffer = malloc(size + 1); //Add one for null termination.
  for(int i = 0; i < size + 1; i++){
     buffer[i] = 0;
  }
  char* newline = malloc(1);
  newline[0] = '\n';
  int val = 0;
  while(1){
    if(currentLine == 0 || currentLine->line == 0)
      break;
//    puts("Concat: \n"); puts(currentLine->line); puts("\n");
    memcpy(buffer + val, currentLine->line, strlen(currentLine->line));
    val += strlen(currentLine->line);
    buffer[val] = '\n';
    val++;
    if(val > size){
        putdec(val); puts(" IS GREATER THAN "); putdec(size);
	puts("\nYOU ARE DUM\n");
    }
//    strcat(buffer, currentLine->line);
//    strcat(buffer,newline);

    currentLine = currentLine->next;
  }
  buffer[size] = 0;
  free(newline);
//  puts("Here we are\n");
  currentLine = head;
  while(currentLine != 0 && currentLine->line != 0){
//	puts("ready to free line\n");
	free(currentLine->line);
	if(currentLine->next == 0){
//		puts("freeing the last one\n");
		free(currentLine);
		break;
	}
	else{
//		puts("freeing one\n");
		currentLine = currentLine->next;
		free(currentLine->prev);
	}
  }
//  puts("buffer is: \n");
  puts(buffer);
  //Write that buffer back to disk
  buffer[size] = 0;
  write(fd, buffer, size);
  free(buffer);
  return 0;
}

int main(int argc, char** argv){
  if(argc > 2){
    puts("ERROR: One must only call edit with one file. Not two, not four, but one. Or zero.\n");
    return -1;
  }
  clear();
  if(argc == 2){
   // char c = 'a';
    int fd = open(argv[1]);
    if(fd < 0){
      puts("ERROR: Unable to open file ");
      puts(argv[1]);
      puts(".\n");
      return -1;
    }
    int len = getlen(fd);
    char* buffer = malloc(len);
    int read = readFully(fd, (void*)buffer, len);
    if(read < 0){
      puts("Read Error.\n");
      return -1;
    }

    struct listNode* head = (struct listNode*)malloc(sizeof(struct listNode));
    currentLine = head;
    currentLine->next = 0;
    currentLine->prev = 0;
    currentLine->line = 0;

    //This reads the file in to the linked list structure.
    int oldHead = 0;
    int oldTail = 0;
    while(oldTail < len){
      if(buffer[oldTail] == '\n'){
        currentLine->line = malloc(oldTail-oldHead + 1);
        memcpy(currentLine->line, (void*)&(buffer[oldHead]), oldTail-oldHead);
	currentLine->line[oldTail-oldHead] = 0;
        oldHead = oldTail + 1;

        currentLine->next = (struct listNode*)malloc(sizeof(struct listNode));
        currentLine->next->prev = currentLine;
        currentLine->next->next = 0;
        currentLine = currentLine->next;

      }
      //putchar(buffer[oldTail]);
      oldTail++;
    }
    currentLine = currentLine->prev; // Make sure that the current line is the last line that was read in.
    free(currentLine->next);
    free(buffer);
    currentLine->next = 0;
    //puts("BEFORE WRITE: \n");
    //printHead(head);
    display(head);
    setCursor(0,0);
    offset = 0;
    //char* gotten = gets();
    getbuf(head, 0);
    close(fd);
    //currentLine->line = append(currentLine->line, gotten, strlen(currentLine->line)); //The strlen appends to the end of the line.

    //TODO This is where I need to actually move the cursor and 
    //     append in the middle of strings and things.
    //     I'm gonna write a display method though, I think.
    

    //This prints out the linked list, debugging purposes.
    /*currentLine = head;
    while(1){
      if(currentLine == 0 || currentLine->line == 0)
        break;
      puts(currentLine->line);
      puts("\n");
      currentLine = currentLine->next;
    }*/


    
    /*while(c != 'p'){
      currentLine->line = 0;
      currentLine->next = 0;
      currentLine->prev = last;

      currentLine->line = gets();

      last = currentLine;

      currentLine->next = (struct listNode*)malloc(sizeof(struct listNode));
      currentLine->next->prev = currentLine;
      currentLine->next->next = 0;
      currentLine->next->line = 0;
      c = *(currentLine->line);
      currentLine = currentLine->next;
    }
    currentLine = head;
    while(1){
      if(currentLine == 0 || currentLine->line == 0)
        break;
      puts(currentLine->line);
      puts("\n");
      currentLine = currentLine->next;
    }

    puts(buffer);
    gets();*/


  }
  if(argc == 1){
    char c = 'a';
    struct listNode* head = (struct listNode*)malloc(sizeof(struct listNode));
    struct listNode* last = 0;
    currentLine = head;
    while(c != 'p'){
      //Create new buffer, not associated with any file yet.
      currentLine->line = 0;
      currentLine->next = 0;
      currentLine->prev = last;

      currentLine->line = gets();

      last = currentLine;

      currentLine->next = (struct listNode*)malloc(sizeof(struct listNode));
      currentLine->next->prev = currentLine;
      currentLine->next->next = 0;
      currentLine->next->line = 0;
      c = *(currentLine->line);
      currentLine = currentLine->next;
    }
    currentLine = head;
    while(1){
      if(currentLine == 0 || currentLine->line == 0)
        break;
      puts(currentLine->line);
      puts("\n");
      currentLine = currentLine->next;
    }
  }

  puts("end of edit!\n");
  return 0;
}

char* getbuf(struct listNode* current, struct listNode* head, long fd) {
  struct listNode* currentLine = current;
  struct listNode* myLine = current;
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
      if(offset > 0){
        putchar(~0xf);
        offset--;
      }
      continue;
    }
    if(c == ~0xe){ //Right Arrow == 0xf1
      if(offset < strlen(myLine->line)){
        putchar(~0xe);
        offset++;
      }
      continue;
    }
    if(c == ~0xd){ //Up Arrow == 0xf2
      if(myLine->prev == 0)
        continue;
      int numLines = numberOfLines(myLine->prev->line);
      int j;
      if(myLine->prev != 0){
        myLine = myLine->prev;
      }
      for(j = 0; j<numLines; j++){
        int i =  putchar(~0xd);
        if(i == 1){ //This is a shift down.
          if(currentLine->prev != 0){
            currentLine = currentLine->prev;
            int x = getRow();
            int y = getColumn();
            display(currentLine);
            setCursor(x,y);
          }
          /*if(currentTopLine->prev != 0){
            currentTopLine = currentTopLine->prev;
            display(currentTopLine);
            }*/
        }
      }
      continue;
    }
    if(c == ~0xc){ //Down Arrow == 0xf3
      if(myLine->next == 0)
        return 0;
      int numLines = numberOfLines(myLine->line);
      int j;
      if(myLine->next != 0){
        myLine = myLine->next;
      }
      for(j = 0; j<numLines; j++){
        int i = putchar(~0xc);
        //putdec(onNonprintedLine(currentLine,myLine));
        //gets();
        if(i == 2 || onNonprintedLine(currentLine, myLine)){ //This is a shift up.
          if(currentLine->next != 0){
            currentLine = currentLine->next;
            int z = numberOfLines(currentLine->prev->line);
            int x = getRow();
            int y = getColumn();
            display(currentLine);
            setCursor(x-z,y);
          }
          /*if(currentTopLine->next != 0){
            currentTopLine = currentTopLine->next;
            display(currentTopLine);
            }*/
        }
      }
      continue;
    }
    if(c == ~0x1){ //^q == 0xfe EXIT
      clear();
      writeFile(fd,head);
      return 0;
    }
    if(c == ~0x2){ //^w == 0xfd
      puts("***");
      puts(myLine->line);
      puts("\n");
      continue;
    }
    if(c == 8 || c == 0x7f){ //Backspace
      if(offset == 0){
        if(myLine->prev != 0 && myLine->prev->line != 0){ //Here, we need to remove the newline. make two node
          int len = strlen(myLine->prev->line);
          int x = getRow();
          myLine->prev->line = append(myLine->prev->line, myLine->line, strlen(myLine->prev->line)); //Add current line to prev
          struct listNode* toDelete = myLine;
          struct listNode* next = myLine->next;
          myLine = myLine->prev;
          myLine->next = next;
          next->prev = myLine;
          free(toDelete->line);
          free(toDelete);
          puts(myLine->line);
          display(currentLine);
          offset = len;
          setCursor(x-1, len);
        }
        continue;
      }
      myLine->line = deleteChar(myLine->line, --offset );
      int x = getRow();
      int y = getColumn();
      display(currentLine);
      setCursor(x,y-1);
      /*//puts(p);
      if(i > 0){
        putchar(c);
        p[i--] = 0;
      }*/
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
