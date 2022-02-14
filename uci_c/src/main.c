#include "stdio.h"
#include </usr/local/include/uci.h>
#include <string.h>
#include <stdlib.h>
//删除一个匿名的section 应该是存着设备信息
//通过section中的option的value删除自身所在的section
int delect_one_anonsection(const char *aDeviceCid)

{
    struct uci_context *ctx = uci_alloc_context(); //申请一个uci上下文
    struct uci_package *pkg = NULL;
    struct uci_element *e;
    struct uci_ptr ptr;
    struct uci_section *sec;
    struct uci_section *s;
    const char *value = NULL;
    int ret = 0;

    if (UCI_OK != uci_load(ctx, "/etc/config/hello", &pkg))
    {
        uci_free_context(ctx);

        return -1;
    }

    //遍历uci配置文件的每一个节

    uci_foreach_element(&pkg->sections, e)
    {

        s = uci_to_section(e);

        value = uci_lookup_option_string(ctx, s, "agent"); //得到lanid

        if ((strcmp("system_setion", s->type) == 0) && (strcmp(aDeviceCid, value) == 0)) //进行节点类型删选 类型和canid匹配
        {

            printf("section name is %s\n", s->e.name);
            printf("type name is %s\n", s->type);

            sec = uci_lookup_section(ctx, pkg, s->e.name); //查找节点
            if (sec != NULL)
            {
                printf("find sec ok\n");
                memset(&ptr, 0, sizeof(struct uci_ptr));
                ptr.package = "hello";
                ptr.section = s->e.name;
                ptr.p = pkg;
                ptr.s = sec;
                ret = uci_delete(ctx, &ptr);
                if (ret != 0)
                {
                    printf("uci_delete exe error\n");
                    ret = -2;
                    goto cleanupd;
                }
                uci_save(ctx, pkg);
                if (ret != 0)
                {
                    printf("uci_save exe error\n");

                    ret = -2;
                    goto cleanupd;
                }
                uci_commit(ctx, &pkg, false);
                if (ret != 0)
                {
                    printf("uci_commit exe error\n");

                    ret = -2;
                    goto cleanupd;
                }
                goto cleanupd;
            }
            else
            {
                printf("find sec fail\n");

                goto cleanupd;
            }
        }
    }

cleanupd:

    uci_unload(ctx, pkg);

    uci_free_context(ctx);
    return ret;
}
/*
 *在配置文件中新增一个节点,注意必须保证section存在
 */
int add_new_section(const char *packag,const char *section)
{	
	char acSsid[32] = {'0'};   //存储输入的ssid
	char acPasswd[32] = {'0'};  //ssid对应的密码
	char acEncryption[8] = {'0'};
	
	struct uci_context *ctx = NULL;
	struct uci_package *pkg = NULL;
	
	//申请一个uci上下文
	ctx = uci_alloc_context();
	
	// 打开配置uci文件
	if(UCI_OK != uci_load(ctx, packag, &pkg))
		return -1;
	
	//添加新的节点
	if((pkg = uci_lookup_package(ctx, packag)) != NULL)
	{
		struct uci_ptr ptr = {
			.p = pkg
		};
		
		// 在打开的uci配置文件中新增节点配置 
		uci_add_section(ctx, pkg, section, &ptr.s);
		
		ptr.o = NULL;
		ptr.option = "device";
		ptr.value = "radio0";   //默认每个设备只有一张无线网卡
		uci_set(ctx, &ptr);
		
		ptr.o = NULL;
		ptr.option = "network";
		ptr.value = "lan";
		uci_set(ctx, &ptr);
		
		ptr.o = NULL;
		ptr.option = "mode";
		ptr.value = "ap";   //诱导器工作方式下，网卡的默认工作模式为ap
		uci_set(ctx, &ptr);
		
		ptr.o = NULL;
		ptr.option = "encryption";
		ptr.value = acEncryption;
		uci_set(ctx, &ptr);
		
		ptr.o = NULL;
		ptr.option = "ssid";
		ptr.value = acSsid;
		uci_set(ctx, &ptr);
		
		if(strcmp(acEncryption, "none"))
		{
			ptr.o = NULL;
			ptr.option = "key";
			ptr.value = acPasswd;
			uci_set(ctx, &ptr);
		}
		
		uci_commit(ctx, &ptr.p, false);
		uci_unload(ctx, ptr.p);//load成功后采用unload，否则不用
	}
	uci_free_context(ctx);
	return 0;
}
/*
 * 按照节点的名称删除该节点 必须是由名section
 * 即删除一个ssid
 */
int delete_section(const char *packag,const char *acSection)
{
    int ret;
    struct uci_package* pkg;
    struct uci_section* sec;
    struct uci_ptr ptr;
    char* err_str = NULL;
    char err_msg[256];
	struct uci_context *ctx = NULL;
	
	
    if(ctx == NULL)
    {
        ctx = uci_alloc_context();  //申请uci上下文
        if(ctx==NULL)
        {
            printf("Failed to alloc uci context");
            // fprintf(stderr, "[%s] %s\n", __func__, err_msg);
            return -1;
        }
    }
 
    ret = uci_load(ctx,packag,&pkg);
    if(ret != 0 || pkg == NULL)
    {
        snprintf(err_msg, sizeof(err_msg), "Failed to load package: '%s' with error",packag);
        goto error;
    }
 
    sec = uci_lookup_section(ctx, pkg, acSection); //查找节点
    if(sec != NULL)
    {
        memset(&ptr, 0, sizeof(struct uci_ptr));
        ptr.package = packag;
        ptr.section = acSection;
        ptr.p = pkg;
        ptr.s = sec;
 
        ret = uci_delete(ctx, &ptr);
        if(ret != 0)
        {
            snprintf(err_msg, sizeof(err_msg), "Failed to delete section: '%s' with error", acSection);
            goto error;
        }
 
        uci_save(ctx, pkg);
        uci_commit(ctx, &pkg, false);
    }
    else
    {
        sprintf(err_msg,"Failed to delete section: '%s' with error section not found",acSection);
        goto error;
    }
 
    uci_unload(ctx, pkg);
	uci_free_context(ctx);
	
	return 0;
	

error:
    uci_unload(ctx, pkg);
    uci_free_context(ctx);
    uci_get_errorstr(ctx, &err_str, err_msg);
    printf("%s\n",err_str);
    free(err_str);
    return -1;
}
/*
 *删除某一个节点的一个option选项 section必须为名称
 */
static int uci_delete_option(const char *packag,const char* section,const char* option)
{
    int ret;
    struct uci_package* pkg;
	struct uci_context *ctx = NULL;
    struct uci_ptr ptr;
    char* err_str = NULL;
    char err_msg[256];
    //1.ctx 必要
    if(ctx == NULL)
    {
        ctx = uci_alloc_context();
        if(ctx == NULL)
        {
            printf("uci_delete_option ctx alloc failed!\n");
            return -1;
        }
    }
 
    //2 pkg 必要
    ret = uci_load(ctx,packag,&pkg);
    if(ret != 0 || pkg == NULL)
    {
        snprintf(err_msg, sizeof(err_msg), "Failed to load package: '%s' with error", packag);
        goto error_pkg;
    }
    
    memset(&ptr,0,sizeof(struct uci_ptr));
    //3. ptr
    ptr.package = packag;
    ptr.section = section;
    ptr.option = option;
 
    ret = uci_delete(ctx,&ptr);
 
    uci_save(ctx, pkg);
    uci_commit(ctx, &pkg, false);
    uci_unload(ctx, pkg);
    uci_free_context(ctx);
    return 0;
 


error_pkg:
    uci_unload(ctx, pkg);
    uci_free_context(ctx);
    uci_get_errorstr(ctx, &err_str, err_msg);
    printf("%s\n",err_str);
    free(err_str);
    return -1;
}
/*
 *修改具名节点option值 注意 section传入的必须是由具体名字的section，不能是type 也不能是 @type[x] 这种类型
 */
static int uci_set_option_string(const char *packag,const char *section, const char *option, const char *value)
{
    int ret;
    struct uci_context *ctx = NULL;
    struct uci_package* pkg;
    struct uci_section* sec;
    struct uci_ptr ptr;
    char* err_str = NULL;
    char err_msg[256] = {'\0'};
 
    //1. ctx 必要
    if(ctx == NULL)
    {
        ctx = uci_alloc_context();  //申请一个uci上下文
        if(ctx==NULL)
        {
            snprintf(err_msg, sizeof(err_msg),"Failed to alloc uci context");
            return -1;
        }
    }
 
    //2. pkg 必要
    ret = uci_load(ctx, packag, &pkg);
    if(ret !=0 || pkg == NULL)
    {
        snprintf(err_msg, sizeof(err_msg),"Failed to load package: '%s' with error",packag);
        goto error_pkg;
    }
 
    // //3. section 非必要，指示为了确定该section存在
    sec = uci_lookup_section(ctx, pkg, section);
    if(sec == NULL)
    {
        snprintf(err_msg, sizeof(err_msg), "Failed to find section: '%s'", section);
        goto error_msg;
    }
 
    // memset(&ptr, 0, sizeof(struct uci_ptr));
 
    //4. ptr
    ptr.package = packag;
    ptr.section = section;
	ptr.option = option;
	ptr.value = value;
	
    // ptr.p = pkg;
    // ptr.s = sec;
 
    ret = uci_set(ctx, &ptr);
    if(ret != 0)
    {
        snprintf(err_msg, sizeof(err_msg), "Failed to set option: '%s' with error", option);
        goto error_uci;
    }
 
    uci_save(ctx, pkg);
    uci_commit(ctx, &pkg, false);
    uci_unload(ctx, pkg);
    uci_free_context(ctx);
    return 0;
 
error_uci:
    uci_unload(ctx, pkg);
    uci_free_context(ctx);

error_pkg:
    uci_get_errorstr(ctx, &err_str, err_msg);
    printf("%s\n",err_str);
    free(err_str);
    return -1;
error_msg:
    printf("%s\n",err_msg);

    uci_unload(ctx, pkg);
    uci_free_context(ctx);
    return -1;
}
void main(void)
{
    int ret = 0;

    //操作位于/etc/config/xxxxpkg 注意执行命令时必须使用sudo，否则不成功
    // delect_one_anonsection("lf");
    // uci_set_option_string("hello","globe","agent","555559999");
    // uci_set_option_string("ptl_config","gw_0","mode","debug111");
    // uci_delete_option("ptl_config","gw_0","test");
    // delete_section("hello","globe");
    add_new_section("hello","new_section");



    return;

    struct uci_context *_ctx = uci_alloc_context(); //申请上下文
    struct uci_ptr ptr = {
        .package = "hello",
        .section = "globe",
        .option = "agent",
        .value = "1111",
    };

    ret = uci_set(_ctx, &ptr); //写入配置
    if (ret != 0)
    {
        printf("uci_set exe error\n");
    }
    // uci_save(_ctx, pkg);
    ret = uci_commit(_ctx, &ptr.p, true); //提交保存更改
    if (ret != 0)
    {
        printf("uci_commit exe error\n");
    }
    // uci_commit(ctx, &pkg, false);
    ret = uci_unload(_ctx, ptr.p); //卸载包
    if (ret != 0)
    {
        printf("uci_unload exe error\n");
    }

    uci_free_context(_ctx); //释放上下文
}
