#include "libc.h"

unsigned int m_z = 641;
unsigned int m_w = 947;

unsigned int getRandom(){
	m_z = 36969 * (m_z & 65535) + (m_z >> 16);
	m_w = 18000 * (m_w & 65535) + (m_w >> 16);
	int returnVal = ((m_z << 16) + m_w);
	return returnVal % 78;
}

int main(int argc, char** argv) {
    char* arr = malloc(80);
    for(int i = 0; i < 80; i++){
	arr[i] = 0;
    }
    clear();
    for(int k = 0; k < 200; k++){
	for(int j = 0; j < 20; j++){
		unsigned int val = (getRandom() + 1);
		if(arr[val] == 0 && (arr[val+1] == 0 || arr[val-1] == 0)){
		    arr[val] = ((getRandom()) % 26) + 65;
		}
		else{
		    arr[val] = 0;
		}
	}
	moveToZero();
	for(int i = 0; i < 80; i++){
	    if(arr[i] != 0){
		putcolor(arr[i], 0, 2);
	    }
	    else{
		putcolor(' ', 0, 2);
	    }
	}
	sleep(50);
    }
    clear();
    return 0;
}
