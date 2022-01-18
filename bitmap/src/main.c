#include "stdio.h"
typedef unsigned char uint8_t;

//位域测试 https://www.cnblogs.com/balingybj/p/4780358.html
typedef struct bs     
{     
    unsigned int a:1;     
    unsigned int b:3;     
    unsigned int c:4;     
} bs_t;

bs_t bit,*pbit;     





int main(int argc, char *argv[])
{

	bit.a=1;     
	bit.b=7; //注意：位域的赋值不能超过该域所能表示的最大值，如b只有3位，能表示的最大数为7，若赋为8，就会出错   
	bit.c=15;
	printf("%d,%d,%d\n",bit.a,bit.b,bit.c);
	pbit=&bit;
	pbit->a=0;
	pbit->b&=3;
	pbit->c=1;
	printf("%d,%d,%d\n",pbit->a,pbit->b,pbit->c);

}