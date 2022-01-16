#include "dllist.h"
#include <stdio.h>
#include <stdlib.h>
 struct student
{
    struct dl_list list;//暂且将链表放在结构体的第一位
    int ID;
    int math;   
};


struct student *p;
struct student *q;
struct student stu1;
struct student stu2;  
struct student *pos;
struct student new_obj={.ID=100,.math=100}; 
void main(void)
{


    printf("linux_list test\n");

    /**************************初始化******************************/
    dl_list_init(&stu1.list);
    dl_list_init(&stu2.list);


    /**************************新增******************************/
    //头插法创建stu stu1链表
     for (int i = 0;i < 6;i++) {
         p = (struct student *)malloc(sizeof(struct student));
         p->ID=i;
         p->math = i;
         //头插法 后进先出
         dl_list_add(&stu1.list,&p->list);
     }

    printf("dl_list_add stu1: \r\n");
    dl_list_for_each(pos, &stu1.list,struct student,list) {
        printf("ID = %d,math = %d\n",pos->ID,pos->math);
    }


    //头插法创建stu stu2链表
     for (int i = 0;i < 3;i++) {
         p = (struct student *)malloc(sizeof(struct student));
         p->ID=i;
         p->math = i;
         //头插法 后进先出
         dl_list_add(&stu2.list,&p->list);
     }

    printf("dl_list_add stu2: \r\n");
    dl_list_for_each(pos, &stu2.list,struct student,list) {
        printf("ID = %d,math = %d\n",pos->ID,pos->math);
    }


    //尾巴插法创建stu stu2链表
    //  for (int i = 0;i < 6;i++) {
    //      p = (struct student *)malloc(sizeof(struct student));
    //      p->ID=i;
    //      p->math = i;
    //      //尾插法 先进先出
    //      dl_list_add_tail(&stu2.list,&p->list);
    //  }

    // printf("dl_list_add_tail: \r\n");
    // dl_list_for_each(pos, &stu2.list,struct student,list) {
    //     printf("ID = %d,math = %d\n",pos->ID,pos->math);
    // }


    // /**************************删除******************************/
    // dl_list_for_each(pos, &stu1.list,struct student,list) {
    //     if(pos->ID == 4)
    //     {
    //         dl_list_del(&(pos->list));
    //         free(pos); //为什么free没有效果？
    //         printf("ID = %d,math = %d\n",pos->ID,pos->math);

    //         break;//注意删除以后不能再循环  必须break跳出循环
    //     } 
    // }

    /**************************替换******************************/
    // dl_list_for_each(pos, &stu1.list,struct student,list) {
    //     if(pos->ID == 4)
    //     {
    //         dl_list_replace_init(&pos->list,&new_obj.list);
    //         break;//注意删除以后不能再循环  必须break跳出循环
    //     } 
    // }

    // /**************************链表删除并插入节点******************************/
    // //把链表中的某一个提升到最前边
    // dl_list_for_each(pos, &stu1.list,struct student,list) {
    //     if(pos->ID == 1)
    //     {
    //         dl_list_move(&pos->list,&stu1.list);
    //         break;//注意删除以后不能再循环  必须break跳出循环
    //     } 
    // }

    /**************************链表删除并插入节点******************************/
    // //把链表中的某一个提升到最后边
    // dl_list_for_each(pos, &stu1.list,struct student,list) {
    //     if(pos->ID == 3)
    //     {
    //         dl_list_move_tail(&pos->list,&stu1.list);
    //         break;//注意删除以后不能再循环  必须break跳出循环
    //     } 
    // }
    printf("dl_list res: \r\n");
    dl_list_for_each(pos, &stu1.list,struct student,list) {
        printf("ID = %d,math = %d\n",pos->ID,pos->math);
    }

    /**************************合并链表******************************/
    // dl_list_splice(&stu2.list,&stu1.list);
    // printf("list_splice res\r\n");
    // dl_list_for_each(pos, &stu1.list,struct student,list) {

    //     printf("stu2 ID = %d,math = %d\n",((struct student*)pos)->ID,((struct student*)pos)->math);
    // }
    



}