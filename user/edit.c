#include "libc.h"
#include "libstr.c"

struct listNode {
  char* line;
  struct listNode* next;
  struct listNode* prev;
};

struct listNode* currentLine;

int numberOfLines(char* line){
  return strlen(line)/80 + 1;
}

int writeFile(int fd, struct listNode* head){
  puts("01\n");

  int size = 0;
  puts("02\n");

  //Tally up the size
  struct listNode* currentLine;
  puts("03\n");
  currentLine = head;
  puts("04\n");
  while(1){
  puts("05\n");
    if(currentLine == 0 || currentLine->line == 0)
      break;
  puts("06\n");

    size += strlen(currentLine->line);
  puts("07\n");
    size += 1; //Add a char for the '\n' that goes between lines;
  puts("08\n");
    
    currentLine = currentLine->next;
  puts("09\n");
  }
  puts("size:");
  putdec(size);
  puts("\n");

  //Create a buffer of correct size
  puts("11\n");
  char* buffer = malloc(size + 1); //Add one for null termination.
  puts("12\n");
  currentLine = head;
  puts("13\n");
  while(1){
  puts("14\n");
    if(currentLine == 0 || currentLine->line == 0)
      break;
  puts("15\n");

    char* newline = malloc(2);
  puts("16\n");
    newline[0] = '\n';
  puts("17\n");
    newline[1] = 0;
  puts("18\n");

    strcat(buffer, currentLine->line);
  puts("19\n");
    strcat(buffer,newline);
  puts("20\n");

    currentLine = currentLine->next;
  puts("21\n");
  }

  //Write that buffer back to disk
  puts("22\n");
  buffer[size] = 0;
  puts("23\n");
  write(fd, buffer, size);
  puts("24\n");
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
    struct listNode* last = 0;
    currentLine->next = 0;

    //This reads the file in to the linked list structure.
    int oldHead = 0;
    int oldTail = 0;
    while(oldTail < len){
      if(buffer[oldTail] == '\n'){
        currentLine->line = malloc(oldTail-oldHead + 1);
        memcpy(currentLine->line, (void*)&(buffer[oldHead]), oldTail-oldHead);
        oldHead = oldTail + 1;

        currentLine->next = (struct listNode*)malloc(sizeof(struct listNode));
        currentLine->next->prev = currentLine;
        currentLine->next->next = 0;
        currentLine->prev = last;
        currentLine = currentLine->next;

      }
      //putchar(buffer[oldTail]);
      oldTail++;
    }
    currentLine = currentLine->prev; // Make sure that the current line is the last line that was read in.
    char* gotten = gets();
    currentLine->line = append(currentLine->line, gotten, strlen(currentLine->line)); //The strlen appends to the end of the line.

    //TODO This is where I need to actually move the cursor and 
    //     append in the middle of strings and things.
    //     I'm gonna write a display method though, I think.
    
    writeFile(fd,head);


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
