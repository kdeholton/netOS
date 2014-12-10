#include "libc.h"

long strlen(const char* str) {
  long n = 0;
  while (*str++ != 0) n++;
  return n;
}

char* strdup(char* str) {
  if (str == 0) return 0;

  long len = strlen(str);
  /* how long is too long? */

  char* out = malloc(len+1);;
  memcpy(out,str,len);
  out[len] = 0;
  return out;
}

char* trimSpace(char* strr){
  {
    //free(strr);
    char* str = malloc(strlen(strr)+1);
    memcpy(str,strr,strlen(strr));
    str[strlen(strr)] = 0;
    char c = str[0];
    int z = 0;
    while(c != '\0'){
      if(c == '\t')
        str[z] = ' ';
      z++;
      c = str[z];
    }
    char* originalstr = str;
    int len = strlen(str);
    while((*str) == ' ') str++;
    char* end = originalstr + len - 1;
    while(end > str && (*end) == ' ') end--;
    end++;

    int size = (end - str) < len ? (end - str) : len;

    char* newstr = malloc(size + 1);
    memcpy(newstr, str, size);
    newstr[size] = 0;

    len = strlen(newstr);
    int i=0,j=0;
    char *output = newstr;
    for (; i<len;)          
    {
      if (newstr[i]!=' '){
        output[j]=newstr[i];
      }
      else
        if(newstr[i-1] != ' '){
          output[j] = ' ';
        }
        else
          j--;
      i++;
      j++;
    }
    output[j]=0;
    return output;
  }
}

char** tokenize(char* str, int* argc){
  str = trimSpace(str);

  if(*str == 0)
    return 0;

  int length = 1;
  int i = 0;
  char c = str[0];
  while(c != '\0'){
    if(c == ' ') length++;
    i++;
    c = str[i];
  }

  char** tokens = malloc(length * 4);

  int start = 0;
  for(i=0; i<length; i++){
    char* s = malloc(strlen(str)+1);
    c = str[start];
    int j=0;
    while(c != ' ' && c != 0){
      s[j] = str[start];
      start ++;
      c = str[start];
      j++;
    }
    s[j] = '\0';
    tokens[i] = s;
    start ++;

  }
  //puts("Just before returning from tokenize:\n");
  /*for(i=0; i<length; i++){
    puts(tokens[i]);
    puts("\n");
    }*/
  *argc = length;
  return tokens;

}

int strEquals (const char *str1, const char *str2) {
  const unsigned char *p1 = (const unsigned char *)str1;
  const unsigned char *p2 = (const unsigned char *)str2;

  while (*p1 != '\0') {
    if (*p2 == '\0') return  1;
    if (*p2 > *p1)   return -1;
    if (*p1 > *p2)   return  1;

    p1++;
    p2++;
  }

  if (*p2 != '\0') return -1;

  return 0;
}

void strcpy(char* dest, const char* source) {
  int i = 0;
  while (1) {
    dest[i] = source[i];
    if (dest[i] == '\0') break;
    i++;
  }
}

void strcat(char *dest, const char *src)
{
  /*strcpy(dest + strlen(dest), src);
  return dest;*/
  int i,j;
  for (i = 0; dest[i] != '\0'; i++)
    ;
  for (j = 0; src[j] != '\0'; j++)
    dest[i+j] = src[j];
  dest[i+j] = '\0';
  return;
  //return dest;
}

char * strncpy(char *dest, const char *src, int n)
{
  int i;

  for (i = 0; i < n && src[i] != '\0'; i++)
    dest[i] = src[i];
  for ( ; i < n; i++)
    dest[i] = '\0';

  return dest;
}

//This returns a new string with 'insert' added at 'pos' in 'subject'.
char* append(char* subject, const char* insert, int pos) {
  char* buf = malloc(strlen(subject) + 2);

  strncpy(buf, subject, pos); // copy at most first pos characters
  int len = strlen(buf);
  strcpy(buf+len, insert); // copy all of insert[] at the end
  len += strlen(insert);  // increase the length by length of insert[]
  strcpy(buf+len, subject+pos); // copy the rest

  //free(subject);
  return buf;
  //strcpy(subject, buf);   // copy it back to subject
}
