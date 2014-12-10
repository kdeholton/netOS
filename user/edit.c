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

    int oldHead = 0;
    int oldTail = 0;
    while(oldTail < len){
      if(buffer[oldTail] == '\n'){
        //oldTail--; //Don't care about the '\n'
        currentLine->line = malloc(oldTail-oldHead + 1);
        memcpy(currentLine->line, (void*)&(buffer[oldHead]), oldTail-oldHead);
        oldHead = oldTail + 1;

        currentLine->next = (struct listNode*)malloc(sizeof(struct listNode));
        currentLine->next->prev = currentLine;
        currentLine->next->next = 0;
        currentLine->prev = last;
        currentLine = currentLine->next;

        //oldTail++; //Skip over the '\n' we didn't care about before
      }
      //putchar(buffer[oldTail]);
      oldTail++;
    }

    currentLine = head;
    while(1){
      if(currentLine == 0 || currentLine->line == 0)
        break;
      puts(currentLine->line);
      puts("\n");
      currentLine = currentLine->next;
    }


    
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
