/*   queue.c  */

#include "queue.h"

void queue_creat(struct list_head *list)
{
    INIT_LIST_HEAD(list);
}

void in_queue(struct list_head *new, struct list_head *head)
{
    list_add_tail(new,head);
}

void out_queue(struct list_head *head)
{
    struct list_head *list = head->next; /* 保存链表的最后节点 */
    
    list_del(head->next);/* 头删法 */
    
    INIT_LIST_HEAD(list); /* 重新初始化删除的最后节点，使其指向自身 */

}

int get_queue_size(struct list_head *head)
{ 
    struct list_head *pos;
    int size = 0;
    
    if (head == NULL) {
        return -1;
    }
    
    list_for_each(pos,head) {
        size++;
    }

    return size;

}

bool is_empt_queue(struct list_head *head)
{
    return list_empty(head);
}