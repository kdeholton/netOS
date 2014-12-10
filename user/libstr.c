#include "libc.h"

long strlen(const char* str) {
    long n = 0;
    while (*str++ != 0) n++;
    return n+1;
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
