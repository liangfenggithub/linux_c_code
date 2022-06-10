#include "stdio.h"
#include "string.h"


typedef unsigned long int uint64_t;

/**
 * @brief C语言利用popen 执行shell并获取shell返回结果
 * 
 * @param Cmd 需要执行的shell
 * @param pbyRtnStr 返回字符串存放位置
 * @return int 
 */
int SystemRtn(const char* Cmd, char* pbyRtnStr)
{
	FILE *fp;
	int rc = 0;
	char result_buf[256];
	int nOnceLen = 0;
	int nTotalLen = 0;
	int i;

	fp = popen(Cmd, "r");
	if(NULL == fp)
	{
		perror("popen执行失败！");
		return -1;
	}

	while(fgets(result_buf, 256, fp) != NULL)
	{
		for (i = 0; i < strlen(result_buf); i++)
		{
			/*把命令返回的换行符去掉*/
			if('\n' == result_buf[i])
			{
				result_buf[i] = '\0';
			}
		}

		nOnceLen = strlen(result_buf);
		memcpy(pbyRtnStr+nTotalLen, result_buf, nOnceLen);
		nTotalLen += nOnceLen;
		//printf("命令[%s] 输出[%s]\r\n", Cmd, result_buf);
		memset(result_buf, 0, nOnceLen);
	}

	*(pbyRtnStr+nTotalLen) = '\0';

	/*等待命令执行完毕并关闭管道及文件指针*/
	rc = pclose(fp);
	if(-1 == rc)
	{
		perror("关闭文件指针失败");
		return -1;
	}
	else
	{
		//printf("命令[%s]子进程结束状态[%d]命令返回值[%d]\r\n", Cmd, rc, WEXITSTATUS(rc));
		return 0;
	}
}

void main(void)
{
    char exe_res_str[100];
    memset(exe_res_str,0,sizeof(exe_res_str));

    //获取当前时间
    SystemRtn("date",exe_res_str);
    printf("linux c exe shell res is: %s\n",exe_res_str);
    // linux c exe shell res is: Wed 23 Mar 2022 03:46:33 AM UTC



    //获取当前地址 linux: 注意openwrt 获取于此不同为：ifconfig | grep br-lan | awk '{ print $5 }'
    SystemRtn("ifconfig | grep ether | awk \'{ print $2 }\'",exe_res_str);
    printf("linux c exe shell res is: %s\n",exe_res_str);
    // linux c exe shell res is: 00:0c:29:8b:f8:cb


    printf("over\n")

}