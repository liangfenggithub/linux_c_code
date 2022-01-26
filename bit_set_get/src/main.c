#include "stdio.h"
#include "bitsnbits.h"

typedef unsigned char uint8_t;



uint8_t bit = 0x35; //0011 0101


int main(int argc, char *argv[])
{

	
	//位控制宏定义测试
	printf("%d,%d,%d\n",_bitXof((uint8_t)bit,0),_bitsXYof((uint8_t)bit,3,1),_bitsXYof((uint8_t)bit,6,4));


}