// #include "iostream.h"
#if 1

#include<stdio.h>
void main(void)
{
    FILE* stream;
    long l;
    float fp;
    char s[81];
    char c;
    unsigned int str_len;
    stream = fopen("fscanf.txt","w+");  // 打开
    if(stream == NULL) // 打开文件失败
    {
        printf("the file is opeaned error!\n");
    }
    else //输出信息
    {
        fprintf(stream,"%s \n %ld \n %f \n %c\n","a_string",6500,3.1415,'x');
        fseek(stream,0L,SEEK_SET); // 定位文件读写指针
        fscanf(stream,"%s",s);

        //输出 文件读写指针
        printf("%ld\n",ftell(stream));
        fscanf(stream,"%ld",&l);
        printf("%ld\n",ftell(stream));
        fscanf(stream,"%f",&fp);
        printf("%ld\n",ftell(stream));
        fscanf(stream," %c",&c);
        printf("%ld\n",ftell(stream));
   
        fclose(stream);  // 关闭
    }
}
#else
#include "stdio.h"
void main(void)
{
    FILE* stream;
    long l;
    float fp;
    char s[81];
    char c;
    stream = fopen("./fscanf.txt","w+");
    if(stream == NULL)  // 打开文件失败
    {
        printf("the file is opeaned error!\n");
    }
    else  // 输出信息
    {
        fprintf(stream,"%s %ld %f %c","a_string",6500,3.1415,'x');  // 格式化
        fseek(stream,0L,SEEK_SET); // 文件定位到文件开始地方
        fscanf(stream,"%s",s);
        fscanf(stream,"%ld",&l);
        fscanf(stream,"%f",&fp);
        fscanf(stream," %c",&c);
        printf("%s\n",s);
        printf("%ld\n",l);
        printf("%f\n",fp);
        printf("%c\n",c);
        fclose(stream);  // 关闭
    }
}
#endif