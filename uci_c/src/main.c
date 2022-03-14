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
int add_new_section(const char *packag, const char *section)
{
    char acSsid[32] = {'0'};   //存储输入的ssid
    char acPasswd[32] = {'0'}; // ssid对应的密码
    char acEncryption[8] = {'0'};

    struct uci_context *ctx = NULL;
    struct uci_package *pkg = NULL;

    //申请一个uci上下文
    ctx = uci_alloc_context();

    // 打开配置uci文件
    if (UCI_OK != uci_load(ctx, packag, &pkg))
        return -1;

    //添加新的节点
    if ((pkg = uci_lookup_package(ctx, packag)) != NULL)
    {
        struct uci_ptr ptr = {
            .p = pkg};

        printf("uci_lookup_package success\n");
        // 在打开的uci配置文件中新增节点配置
        uci_add_section(ctx, pkg, section, &ptr.s);

        //测试增加数据
        int a = 123;
        char b[] = "hehllo";
        char option_buff[50];

        ptr.o = NULL;
        ptr.option = "a";
        sprintf(option_buff,"%x",a);
        ptr.value = option_buff;
        uci_set(ctx, &ptr);

                ptr.o = NULL;
        ptr.option = "b";
        ptr.value = b;
        uci_set(ctx, &ptr);

        // ptr.o = NULL;
        // ptr.option = "device";
        // ptr.value = "radio0"; //默认每个设备只有一张无线网卡
        // uci_set(ctx, &ptr);

        // ptr.o = NULL;
        // ptr.option = "network";
        // ptr.value = "lan";
        // uci_set(ctx, &ptr);

        // ptr.o = NULL;
        // ptr.option = "mode";
        // ptr.value = "ap"; //诱导器工作方式下，网卡的默认工作模式为ap
        // uci_set(ctx, &ptr);

        // ptr.o = NULL;
        // ptr.option = "encryption";
        // ptr.value = acEncryption;
        // uci_set(ctx, &ptr);

        // ptr.o = NULL;
        // ptr.option = "ssid";
        // ptr.value = acSsid;
        // uci_set(ctx, &ptr);

        // if (strcmp(acEncryption, "none"))
        // {
        //     ptr.o = NULL;
        //     ptr.option = "key";
        //     ptr.value = acPasswd;
        //     uci_set(ctx, &ptr);
        // }

        uci_commit(ctx, &ptr.p, false);
        uci_unload(ctx, ptr.p); // load成功后采用unload，否则不用
    }
    uci_free_context(ctx);
    return 0;
}
/*
 * 按照节点的名称删除该节点 必须是由名section
 * 即删除一个ssid
 */
int delete_section(const char *packag, const char *acSection)
{
    int ret;
    struct uci_package *pkg;
    struct uci_section *sec;
    struct uci_ptr ptr;
    char *err_str = NULL;
    char err_msg[256];
    struct uci_context *ctx = NULL;

    if (ctx == NULL)
    {
        ctx = uci_alloc_context(); //申请uci上下文
        if (ctx == NULL)
        {
            printf("Failed to alloc uci context");
            // fprintf(stderr, "[%s] %s\n", __func__, err_msg);
            return -1;
        }
    }

    ret = uci_load(ctx, packag, &pkg);
    if (ret != 0 || pkg == NULL)
    {
        snprintf(err_msg, sizeof(err_msg), "Failed to load package: '%s' with error", packag);
        goto error;
    }

    sec = uci_lookup_section(ctx, pkg, acSection); //查找节点
    if (sec != NULL)
    {
        memset(&ptr, 0, sizeof(struct uci_ptr));
        ptr.package = packag;
        ptr.section = acSection;
        ptr.p = pkg;
        ptr.s = sec;

        ret = uci_delete(ctx, &ptr);
        if (ret != 0)
        {
            snprintf(err_msg, sizeof(err_msg), "Failed to delete section: '%s' with error", acSection);
            goto error;
        }

        uci_save(ctx, pkg);
        uci_commit(ctx, &pkg, false);
    }
    else
    {
        sprintf(err_msg, "Failed to delete section: '%s' with error section not found", acSection);
        goto error;
    }

    uci_unload(ctx, pkg);
    uci_free_context(ctx);

    return 0;

error:
    uci_unload(ctx, pkg);
    uci_free_context(ctx);
    uci_get_errorstr(ctx, &err_str, err_msg);
    printf("%s\n", err_str);
    free(err_str);
    return -1;
}
/*
 *删除某一个节点的一个option选项 section必须为名称
 */
static int uci_delete_option(const char *packag, const char *section, const char *option)
{
    int ret;
    struct uci_package *pkg;
    struct uci_context *ctx = NULL;
    struct uci_ptr ptr;
    char *err_str = NULL;
    char err_msg[256];
    // 1.ctx 必要
    if (ctx == NULL)
    {
        ctx = uci_alloc_context();
        if (ctx == NULL)
        {
            printf("uci_delete_option ctx alloc failed!\n");
            return -1;
        }
    }

    // 2 pkg 必要
    ret = uci_load(ctx, packag, &pkg);
    if (ret != 0 || pkg == NULL)
    {
        snprintf(err_msg, sizeof(err_msg), "Failed to load package: '%s' with error", packag);
        goto error_pkg;
    }

    memset(&ptr, 0, sizeof(struct uci_ptr));
    // 3. ptr
    ptr.package = packag;
    ptr.section = section;
    ptr.option = option;

    ret = uci_delete(ctx, &ptr);

    uci_save(ctx, pkg);
    uci_commit(ctx, &pkg, false);
    uci_unload(ctx, pkg);
    uci_free_context(ctx);
    return 0;

error_pkg:
    uci_unload(ctx, pkg);
    uci_free_context(ctx);
    uci_get_errorstr(ctx, &err_str, err_msg);
    printf("%s\n", err_str);
    free(err_str);
    return -1;
}
/*
 *修改具名节点option值 注意 section传入的必须是由具体名字的section，不能是type 也不能是 @type[x] 这种类型
 */
static int uci_set_option_string(const char *packag, const char *section, const char *option, const char *value)
{
    int ret;
    struct uci_context *ctx = NULL;
    struct uci_package *pkg;
    struct uci_section *sec;
    struct uci_ptr ptr;
    char *err_str = NULL;
    char err_msg[256] = {'\0'};

    // 1. ctx 必要
    if (ctx == NULL)
    {
        ctx = uci_alloc_context(); //申请一个uci上下文
        if (ctx == NULL)
        {
            snprintf(err_msg, sizeof(err_msg), "Failed to alloc uci context");
            return -1;
        }
    }

    // 2. pkg 必要
    ret = uci_load(ctx, packag, &pkg);
    if (ret != 0 || pkg == NULL)
    {
        snprintf(err_msg, sizeof(err_msg), "Failed to load package: '%s' with error", packag);
        goto error_pkg;
    }

    // //3. section 非必要，指示为了确定该section存在
    sec = uci_lookup_section(ctx, pkg, section);
    if (sec == NULL)
    {
        snprintf(err_msg, sizeof(err_msg), "Failed to find section: '%s'", section);
        goto error_msg;
    }

    // memset(&ptr, 0, sizeof(struct uci_ptr));

    // 4. ptr
    ptr.package = packag;
    ptr.section = section;
    ptr.option = option;
    ptr.value = value;

    // ptr.p = pkg;
    // ptr.s = sec;

    ret = uci_set(ctx, &ptr);
    if (ret != 0)
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
    printf("%s\n", err_str);
    free(err_str);
    return -1;
error_msg:
    printf("%s\n", err_msg);

    uci_unload(ctx, pkg);
    uci_free_context(ctx);
    return -1;
}
//查找option的string，或者 list 的string
int lfuci_get_option(struct uci_ptr *target,char *option_value, char **list_value, int *list_len)
{
    int ret;
    struct uci_context *ctx = NULL;
    struct uci_package *pkg;
    struct uci_section *sec;
    struct uci_option *opt;
    struct uci_element *p_uci_list_ele;


    struct uci_ptr ptr;
    char *err_str = NULL;
    char err_msg[256] = {'\0'};

    // 1. ctx 必要
    if (ctx == NULL)
    {
        ctx = uci_alloc_context(); //申请一个uci上下文
        if (ctx == NULL)
        {
            snprintf(err_msg, sizeof(err_msg), "Failed to alloc uci context");
            return -1;
        }
    }

    // 2. pkg 必要
    ret = uci_load(ctx, target->package, &pkg);
    if (ret != 0 || pkg == NULL)
    {
        snprintf(err_msg, sizeof(err_msg), "Failed to load package: '%s' with error", target->package);
        goto error_pkg;
    }

    // 3. section
    sec = uci_lookup_section(ctx, pkg, target->section);
    if (sec == NULL)
    {
        snprintf(err_msg, sizeof(err_msg), "Failed to find section: '%s'", target->section);
        goto error_msg;
    }

    // 4.option
    opt = uci_lookup_option(ctx, sec, target->option);

    if (opt == NULL)
    {
        snprintf(err_msg, sizeof(err_msg), "Failed to find option: '%s'", target->option);
        goto error_msg;
    }

    // 5.value值拷贝
    if (opt->type == UCI_TYPE_STRING)
    {
        printf("type is UCI_TYPE_STRING\n");

        strcpy(option_value, opt->v.string);
    }
    else if (opt->type == UCI_TYPE_LIST)
    {
        printf("type is UCI_TYPE_LIST\n");
        (*list_len) = 0;
        uci_foreach_element(&(opt->v.list), p_uci_list_ele) //遍历list 取得flag的值
        {
            printf("list value: %s\n", p_uci_list_ele->name);
            // LOG_printf(LOG_DBG_UCI_GET_traffic,"[%s][%d] value: %s\n", __FILE__, __LINE__, p_uci_list_ele->name);
            *list_value++ = strdup(p_uci_list_ele->name);
            
            (*list_len)++;
            
        }
    }

    uci_unload(ctx, pkg);
    uci_free_context(ctx);
    return 0;

error_uci:
    uci_unload(ctx, pkg);
    uci_free_context(ctx);

error_pkg:
    uci_get_errorstr(ctx, &err_str, err_msg);
    printf("%s\n", err_str);
    free(err_str);
    return -1;
error_msg:
    printf("%s\n", err_msg);

    uci_unload(ctx, pkg);
    uci_free_context(ctx);
    return -1;
}

//列出指定package 下每个sections的名称
static void list_all_section(const char *package)
{
	struct uci_context *ctx = uci_alloc_context();  //申请一个uci上下文
	struct uci_package *pkg = NULL;
	struct uci_element *e;
	//int i = 0;
	
	
	if(UCI_OK != uci_load(ctx, package, &pkg))
		return;
 	//遍历uci配置文件的每一个节
	uci_foreach_element(&pkg->sections, e)
	{
		struct uci_section *s = uci_to_section(e);
		
		//if(!strcmp("wifi-iface", s->type))   //进行节点类型删选
		//{
			printf("section name is %s\n", s->e.name); 
		//}
	}

	uci_unload(ctx, pkg);
	uci_free_context(ctx);
	ctx = NULL;
}
/**
 * @brief 根据section的类型找下边第一个陪陪的option 对应的值
 * 
 * @param package 
 * @param section_type 
 * @param option 
 * @param option_value 
 * @param list_value 
 * @param list_len 
 * @return int 0 成功 -2 seciton type没找到 -3 option没有找到
 */
int lfuci_get_first_option_by_stype(const char *package, 
                const char *section_type, 
                const char *option,
                char *option_value,
                char **list_value, 
                int *list_len)
{

    int res = -1;
    struct uci_context *ctx = uci_alloc_context();
    const char *pValueData = NULL;
    struct uci_package *pkg = NULL; //对应一个配置文件
    struct uci_option *opt;
    struct uci_element *e;
    struct uci_element *p_uci_list_ele;
    int find_section_type_flag = 0;
    int find_option_flag = 0;



    // 1.pkg
    if (UCI_OK != uci_load(ctx, package, &pkg)) //解析一个uci配置文件并把它存到ctx中
    {
        printf("lfuci_get_option_by_sec_type package load error \n");
        res = -1;
        goto clean;
    }

    // 2.遍历每一个section
    uci_foreach_element(&pkg->sections, e)
    {
        struct uci_section *s = uci_to_section(e);

        printf("seciton type is: %s name is:%s\n",s->type,s->e.name);

        // 3. 比对section类型
        if (0 == strcmp(section_type, s->type))
        {
            find_section_type_flag = 1;
            printf("section type is match\n");
            // 4.option
            if (NULL != (opt = uci_lookup_option(ctx, s, option)))
            {
                find_option_flag = 1;
                // 5.option 类型判断
                if (opt->type == UCI_TYPE_STRING)
                {
                    printf("type is UCI_TYPE_STRING\n");

                    strcpy(option_value, opt->v.string);
                    res = 0;
                    break;

                }
                else if (opt->type == UCI_TYPE_LIST)
                {
                    printf("type is UCI_TYPE_LIST\n");

                    (*list_len) = 0;
                    uci_foreach_element(&(opt->v.list), p_uci_list_ele) //遍历list 取得flag的值
                    {
                        printf("list value: %s\n", p_uci_list_ele->name);
                        // LOG_printf(LOG_DBG_UCI_GET_traffic,"[%s][%d] value: %s\n", __FILE__, __LINE__, p_uci_list_ele->name);
                        *list_value++ = strdup(p_uci_list_ele->name);

                        (*list_len)++;
                    }
                    res = 0;
                    break;
                }

                // printf("get uci cfg ok, %s: %s\n", option, value);

                break;
            }
            else
            {
                printf("option %s is not found\n",option);
            }
        }
    }
    if(find_section_type_flag == 0)
    {
        res = -2;
    }
    else if(find_option_flag == 0)
    {
        res = -3;
    }
clean:
    uci_unload(ctx, pkg); //从ctx中卸载一个配置文件包
    uci_free_context(ctx);
    ctx = NULL;

    return res;
}
void main(void)
{
    int ret = 0;
    char name[50];
    char *list_ptr[10];
    int list_len;

    //操作位于/etc/config/xxxxpkg 注意执行命令时必须使用sudo，否则不成功
    // delect_one_anonsection("lf");
    // uci_set_option_string("hello","globe","agent","555559999");
    // uci_set_option_string("ptl_config","gw_0","mode","debug111");
    // uci_delete_option("ptl_config","gw_0","test");
    // delete_section("hello","globe");
    if(0 == add_new_section("hello","new_section"))
    {
        printf("add_new_section success\n");
    }


    /*
   luke@ubuntu20:/etc/config$ cat hello_2
    config system_setion 'globe'
            option url 'lf.cloud.com'
            option delay '22'
            option agent '555559999a'
            list arr 0
            list arr 2
            list arr 4
    */
    // struct uci_ptr target = {
    //     .package = "hello_2",
    //     .section = "globe",
    //     .option = "url",
    // };

    // list_all_section("ptl_config");
    // if (0 == lfuci_get_option(&target, name,NULL,NULL))
    // {
    //     printf("name is %s\n", name);
    // }

    // target.option = "arr";
    // if (0 == lfuci_get_option(&target, NULL,list_ptr,&list_len))
    // {
    //     for(int i = 0; i< list_len; i++)
    //     {
    //         if(list_ptr[i])
    //         {
    //             printf("list %d is %s\n", i,list_ptr[i]);
    //             free(list_ptr[i]);
    //         }
    //     }
    // }

    // //get by section type
    // if (0 == lfuci_get_first_option_by_stype("hello_2", "system_setion","url",name,NULL,NULL))
    // {
    //     printf("get by section type name is %s\n", name);

    // }


    return;

}
