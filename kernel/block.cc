#include "block.h"
#include "stdint.h"
#include "machine.h"
#include "debug.h"

/***************/
/* BlockDevice */
/***************/

static uint32_t min(uint32_t a, uint32_t b) {
    if (a < b) return a; else return b;
}

uint32_t BlockDevice::read(uint32_t offset, void* buf, uint32_t n) {
    uint32_t sector = offset/blockSize;
    char *data = new char[blockSize];
    readBlock(sector,data);
    uint32_t dataOffset = offset - (sector * blockSize);
    uint32_t m = min(n,blockSize-dataOffset);
    memcpy(buf,&data[dataOffset],m);
    delete[] data;
    return m;  
}

uint32_t BlockDevice::write(uint32_t offset, void* buf, uint32_t len, uint32_t first, uint32_t headerSize){
	if(first){
		char* data = new char[(len + 8)];
		data[0] = 0;
		data[1] = 0;
		data[2] = 0;
		data[3] = 1;
		data[7] = (char)(headerSize >> 24);
		data[6] = (char)((headerSize << 8) >> 24);
		data[5] = (char)((headerSize << 16) >> 24);
		data[4] = (char)((headerSize << 24) >> 24);
		memcpy(&data[8], buf, len);
		uint32_t blockNumber = offset / blockSize;
		writeBlock(blockNumber, data, len+8);
		delete[] data;
	}
	else{
		char* data = new char[len];
		memcpy(data, buf, len);
		uint32_t blockNumber = offset / blockSize;
		writeBlock(blockNumber, data, len);
		delete[] data;
		
	}
	return len;
}

void BlockDevice::readFully(uint32_t offset, void* buf, uint32_t n) {
    uint32_t togo = n;
    char* ptr = (char*) buf;

    while (togo > 0) {
        uint32_t c = read(offset,ptr,togo);
        togo -= c;
        ptr += c;
        offset += c;
    }
}

