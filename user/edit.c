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
  currentLine = head;
  //Create a buffer of correct size
  char* buffer = malloc(size + 1); //Add one for null termination.
  for(int i = 0; i < size + 1; i++){
    buffer[i] = 0;
  }
  int val = 0;
  while(1){
    if(currentLine == 0 || currentLine->line == 0)
      break;
    memcpy(buffer + val, currentLine->line, strlen(currentLine->line));
    val += strlen(currentLine->line);
    buffer[val] = '\n';
    val++;
    if(val > size){
      putdec(val); puts(" IS GREATER THAN "); putdec(size);
      puts("\nYOU ARE DUM\n");
    }

    currentLine = currentLine->next;
  }
  buffer[size] = 0;
  //Write that buffer back to disk
  buffer[size] = 0;
  write(fd, buffer, size);
  free(buffer);
  return 0;
}

void cleanup(struct listNode* head){
  struct listNode* current = head;
  while(current != 0 && current->line != 0){
    free(current->line);
    if(current->next == 0){
      free(current); //Just freed the last node
      break;
    }
    else{
      current = current->next;
      free(current->prev);
    }
  }
}

int main(int argc, char** argv){
  if(argc > 2){
    puts("ERROR: One must only call edit with one file. Not two, not four, but one.\n");
    return -1;
  }
  clear();
  if(argc == 2){
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

    clear();
    puts("Controls:\n\n");
    puts("^w : save to disk\n");
    puts("^q : exit\n");
    puts("\n\n\n\n");
    puts("Press [ENTER] to continue...");
    gets();

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
    display(head);
    setCursor(0,0);
    offset = 0;
    getbuf(head, head, fd);
    close(fd);



  }
  if(argc == 1){ // ERROR, we do not save an unnamed buffer
    return -1;
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
            int x = getRow();
            int y = getColumn();
            currentLine = currentLine->prev;
            display(currentLine);
            setCursor(x,y);
          }
          break;
        }
      }
      if(offset > strlen(myLine->line)){
        int lines = offset / 80;
        offset = strlen(myLine->line);
        setCursor(getRow() - lines + offset/80, offset%80);
      }
      continue;
    }
    if(c == ~0xc){ //Down Arrow == 0xf3
      if(myLine->next == 0)
        continue;
      int numLines = numberOfLines(myLine->line);
      int j;
      if(myLine->next != 0){
        myLine = myLine->next;
      }
      for(j = 0; j<numLines; j++){
        int i = putchar(~0xc);
        if(i == 2 || onNonprintedLine(currentLine, myLine)){ //This is a shift up.
          if(currentLine->next != 0){
            currentLine = currentLine->next;
            int z = numberOfLines(currentLine->prev->line);
            int x = getRow();
            int y = getColumn();
            display(currentLine);
            setCursor(x-z,y);
          }
        }
      }
      if(offset > strlen(myLine->line)){
        int lines = offset/80;
        offset = strlen(myLine->line);
        setCursor(getRow() - lines + offset/80,offset%80);
      }
      continue;
    }
    if(c == ~0x1){ //^q == 0xfe EXIT
      clear();
      cleanup(head);
      return 0;
    }
    if(c == ~0x2){ //^w == 0xfd 
      int x = getRow();
      int y = getColumn();
      clear();
      writeFile(fd,head);
      puts("File written successfully!\n\n\n\n\n");
      puts("Press [ENTER] to continue...");
      gets();
      display(currentLine);
      setCursor(x,y);
      continue;
    }

    if(c == 13){ //Split node into two, changing the linked list structure. NEWLINE, \n
      struct listNode* newNode = (struct listNode*)malloc(sizeof(struct listNode));
      newNode->line = malloc(strlen(myLine->line) - offset + 1);
      newNode->prev = myLine;
      newNode->next = myLine->next;

      memcpy((void*)(newNode->line), (void*)(myLine->line + offset), strlen(myLine->line) - offset);
      newNode->line[strlen(myLine->line) - offset] = 0; //null termination!

      myLine->line = realloc(myLine->line, offset + 1); //resize the old string, cause it's now shorter.
      myLine->line[offset] = 0;

      //Update pointers
      myLine->next = newNode;
      if(newNode->next != 0)
        newNode->next->prev = newNode;
      myLine = newNode;

      offset = 0;

      int x = getRow();
      int diff = 25 - x - numberOfLines(myLine->line);
      int count = 0;
      while(diff <= 0){
        int lines = numberOfLines(currentLine->line);
        diff += lines;
        count += lines;
        currentLine = currentLine->next;
      }
      display(currentLine);
      setCursor(x + 1 - count , 0);
      continue;
    }

    if(c == 8 || c == 0x7f){ //Backspace
      if(offset == 0){
        if(myLine->prev != 0 && myLine->prev->line != 0){ //Here, we need to remove the newline. join two node
          int len = strlen(myLine->prev->line);
          int x = getRow();
          myLine->prev->line = append(myLine->prev->line, myLine->line, strlen(myLine->prev->line)); //Add current line to prev
          struct listNode* toDelete = myLine;
          struct listNode* next = myLine->next;
          myLine = myLine->prev;
          myLine->next = next;
          if(next != 0)
            next->prev = myLine;
          free(toDelete->line);
          free(toDelete);
          if(x == 0){ //Here, we are at the top of the screen, trying to delete. need to scroll back up some.
            currentLine = currentLine->prev;
            if(currentLine == 0); //THIS IS VERY BAD. SHOULD NEVER HAPPEN, BUT BAD NONETHELESS
            display(currentLine);
            offset = len;
            setCursor(x + offset/80, len%80);
          }
          else{
            display(currentLine);
            offset = len;
            setCursor(x-1, len%80);
          }
        }
        continue;
      }
      myLine->line = deleteChar(myLine->line, --offset );
      int x = getRow();
      int y = getColumn();
      display(currentLine);
      if((offset + 1) % 80 == 0){
        setCursor(x-1,79);
      }
      else
        setCursor(x,y-1);
      continue;
    }

    //If we get here, this means we have an actual character to type!!
    //This is where we want to stick it in the buffer of the linked list node.
    if(1){ // for debugging purposes, left in.
      int x = getRow();
      int y = getColumn();
      myLine->line = addChar(myLine->line, offset, c);
      int len = 0;
      if(onNonprintedLine(currentLine,myLine)){
        len = numberOfLines(currentLine->line);
        currentLine = currentLine->next;
      }
      display(currentLine);
      if(y >= 80){
        y = 0;
        x ++;
      }
      setCursor(x - len,y+1);
      offset++;
      continue;
    }
    return 0;
  }
}
