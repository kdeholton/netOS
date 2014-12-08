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

uint32_t BlockDevice::write(uint32_t offset, void* buf, uint32_t len){
	Debug::printf("Size is %d\n", len);
	Debug::printf("first byte is %d\n", len >> 24);
	Debug::printf("Second byte is %d\n", (len << 8) >> 24);
	//Debug::printf("Buf is %s\n", buf);
	char* data = new char[(len + 8) + 2];
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 1;
	data[7] = (char)(len >> 24);
	data[6] = (char)((len << 8) >> 24);
	data[5] = (char)((len << 16) >> 24);
	data[4] = (char)((len << 24) >> 24);
	data[len + 9] = 0;
	data[len + 8] = 0;
	memcpy(&data[8], buf, len);
	Debug::printf("data is %s\n", data + 8);
	uint32_t blockNumber = offset / blockSize;
	Debug::printf("BlocKNumber is %d\n", blockNumber);
	writeBlock(blockNumber, data, len+10);
	//Debug::printf("Wrote the block!!\n");
	//Debug::printf("Writing %s\n", data);
	//delete[] data;
	//Debug::printf("Data was deleted\n");
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

