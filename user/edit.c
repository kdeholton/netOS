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

void printHead(struct listNode* head){
	struct listNode* current = head;
	while(current != 0){
		if(current->line == 0){
			puts("YOU DONE FUCKED UP NOW\n");
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
