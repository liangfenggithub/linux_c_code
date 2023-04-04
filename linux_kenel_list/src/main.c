#include "linux_kenel_list.h"
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
struct student
{
    struct list_head  list;//暂且将链表放在结构体的第一位
    int ID;
    int math;   
};

/* 遍历打印链表 */
int print_student_list(char * pre_info,struct student *stu)
{
	int i=0;
	if(NULL == stu) return -1;
	printf("%s \n",pre_info);
#if 0

    //基本遍历
	struct list_head *pos;		//临时变量，保存链表中指针位置
	struct student *tmp;		//临时变量，保存遍历时，临时的结构体
    list_for_each(pos, &(stu->list)) 	//遍历链表
    // for (pos = (head)->next; pos != (head); pos = pos->next)
    {
        // container_of(ptr, type, member)
		tmp = list_entry(pos, struct student, list);
		printf("[%d]stu->ID :%d \n", i, tmp->ID);
		i++;
	}
#else
    //简化遍历,注意 pos类型不同
	struct student *pos;		//临时变量，保存遍历时，临时的结构体

    list_for_each_entry(pos,&(stu->list),list)
    {
		printf("[%d]stu->ID :%d \n", i, pos->ID);
		i++;
	}
#endif

	return 0;
}

/* 反向遍历打印链表 */
int print_student_list_reverse(char * pre_info,struct student *stu)
{
	int i=0;
	if(NULL == stu) return -1;
	printf("%s \n",pre_info);

    //简化遍历,注意 pos类型不同
	struct student *pos;		//临时变量，保存遍历时，临时的结构体

    list_for_each_entry_reverse(pos,&(stu->list),list)//不安全遍历
    // list_for_each_entry_safe_reverse(pos,&(stu->list),list) //反向安全遍历
    {
		printf("[%d]stu->ID :%d \n", i, pos->ID);
		i++;
	}

	return 0;
}

void list_test(void)
{



    struct student *p;
    struct student *q;
    struct student stu1;
    struct student stu2;  
    struct student *pos;
    struct student *pos_temp;
    struct student new_obj={.ID=100,.math=100}; 

    printf("linux_list test\n");

    /**************************初始化******************************/
    INIT_LIST_HEAD(&stu1.list);
    INIT_LIST_HEAD(&stu2.list);

    /**************************新增******************************/
    //头插法创建stu stu1链表
     for (int i = 0;i < 6;i++) {
         p = (struct student *)malloc(sizeof(struct student));
         p->ID=i;
         p->math = i;
         //头插法 先进后出
         list_add(&p->list,&stu1.list);
     }

    print_student_list("stu1 is :",&stu1);
    print_student_list_reverse("反向遍历 stu1 is :",&stu1);


    //尾巴插法创建stu stu2链表
     for (int i = 0;i < 3;i++) {
         p = (struct student *)malloc(sizeof(struct student));
         p->ID=i;
         p->math = i;
         //头插法 先进先出
         list_add_tail(&p->list,&stu2.list);

     }

    print_student_list("stu2 is :",&stu2);


    /**************************不安全删除,删除指定节点后不能再次遍历循环******************************/
    list_for_each_entry(pos, &(stu1.list),list) {
        if(pos->ID == 4)
        {
            list_del(&(pos->list));
            printf("找到stu1 中 ID = %d 的学生,其math 为 %d 开始删除\n",pos->ID,pos->math);
            free(pos); //为什么free没有效果？
            break;//注意删除以后不能再循环,必须break跳出循环,因为删除后 entry的前向后后向指针分别指向了LIST_POISON1和LIST_POISON2,该值为内核设置的一个区域,再应用层里面设置为了0
        } 
    }
    print_student_list("不安全删除后的 stu1 :",&stu1);


    /**************************安全删除,删除指定节点后可以继续再次遍历循环******************************/
    list_for_each_entry_safe(pos,pos_temp, &(stu1.list),list) {
        if((pos->ID == 3)||(pos->ID == 2))
        {
            // list_del(&(pos->list));//两个删除都可以,区别是删除后指针指向不同
            list_del_init(&(pos->list));//与上一个的删除不同之处在于删除后将指针指回自己

            printf("找到stu1 中 ID = %d 的学生,其math 为 %d 开始删除\n",pos->ID,pos->math);
            free(pos); 
            //注意可以继续执行
        } 
    }
    print_student_list("安全删除后的 stu1 :",&stu1);



    /**************************替换******************************/
    list_for_each_entry_safe(pos,pos_temp, &(stu1.list),list) {
        if(pos->ID == 1)
        {
            printf("找到stu1 中 ID = %d 的学生,将其替换为 ID = 1000\n",1);

            struct student * new_obj;
            new_obj = (struct student *)malloc(sizeof(struct student));
            new_obj->ID=1000;
            new_obj->math = 1000;
            list_replace_init(&pos->list,&new_obj->list); //替换后old重新初始化,使其前驱和后继指向自身。 
            break;
        } 
    }
    print_student_list("替换后的 stu1 :",&stu1);


    /**************************删除list指针所处容器结构节点,并头插添加到另外一个头节点上******************************/
    list_for_each_entry_safe(pos,pos_temp, &stu1.list,list) {
        if(pos->ID == 1000)
        {
            printf("找到stu1中 ID=1000 的节点,头插入 stu2 的链表中\n");
            list_move(&pos->list,&stu2.list);
        } 
    }
    print_student_list("删除后的stu1 :",&stu1);
    print_student_list("插入后的stu2 :",&stu2);

    /**************************删除list指针所处容器结构节点,并尾巴插添加到另外一个头节点上******************************/
    list_for_each_entry_safe(pos,pos_temp, &stu2.list,list) {
        if(pos->ID == 1000)
        {
            printf("找到stu2中 ID=1000 的节点,头插入 stu1 的链表中\n");
            list_move_tail(&pos->list,&stu1.list);
        } 
    }
    print_student_list("删除后的stu2 :",&stu2);
    print_student_list("插入后的stu1 :",&stu1);

    /**************************第一个元素******************************/
    p = list_first_entry(&stu1.list,struct student, list);
    printf("stu1 第一个元素的 ID = %d\n",p->ID);


    /**************************判断链表是否为空******************************/
    printf("stu1 为空吗? %s\n",list_empty(&stu1.list) ? "yes":"no");

    /**************************判断元素是否为链表的最后一个******************************/
    list_for_each_entry_safe(pos,pos_temp, &stu1.list,list) {
        if(pos->ID == 1000)
        {
            printf("stu1 中ID = 1000 的元素是链表中最后一个吗? %s\n",list_is_last(&stu1.list,&p->list) ? "yes":"no");
        } 
    }

    /**************************合并链表******************************/
    // dl_list_splice(&stu2.list,&stu1.list);
    // printf("list_splice res\r\n");
    // dl_list_for_each(pos, &stu1.list,struct student,list) {

    //     printf("stu2 ID = %d,math = %d\n",((struct student*)pos)->ID,((struct student*)pos)->math);
    // }
}


    
void queue_test(void)
{
    struct student head; //注意head本身并未存储数据，第一个数据存储在head.list->next中
    struct student *pos,*n,*p;
    
    //初始化
    queue_creat(&head.list);

    //push
    for (int i = 0; i < 6; i++) {
        p = (struct student *)malloc(sizeof(struct student));
        p->ID=i;
        p->math = i;
        in_queue(&p->list,&head.list);
    }
    print_student_list("queue info is:",&head);

    //pop
    printf("out_queue %d\n",get_queue_head(pos,&head.list,list)->ID);
    out_queue(&head.list);//执行pop操作

    //pop
    printf("out_queue %d\n",get_queue_head(pos,&head.list,list)->ID);
    out_queue(&head.list);//执行pop操作

    //pop
    printf("out_queue %d\n",get_queue_head(pos,&head.list,list)->ID);
    out_queue(&head.list);//执行pop操作
    print_student_list("after pop, queue info is:",&head);

}
void main(void)
{

    // list_test();
   queue_test(); //先进先出的FIFO测试
}


/*安全遍历list_for_each_entry_safe 和不安全遍历list_for_each_entry的注意事项
值得注意的是，如果链表数据域中的元素都相等，使用list_for_each_entry_safe反而会无限循环，list_for_each_entry却能正常工作。但是，在通常的应用场景下，数据域的判断条件不会是全部相同链表，例如在自己使用链表实现的线程中，常用线程名字作为move的条件判断，而线程名字肯定不应该是相同的。所以，具体的内核链表API，需要根据自己的应用场景选择。list_for_each_entry_safe是缓存了下一个节点的地址，list_for_each_entry是无缓存的，挨个遍历，所以在删除节点的时候，list_for_each_entry需要注意，如果没有将删除节点的前驱后继处理好，那么将引发问题，而list_for_each_entry_safe通常不用关心，但是在你使用的条件判断进行move操作时，不应该使用各个节点可能相同的条件。
*/