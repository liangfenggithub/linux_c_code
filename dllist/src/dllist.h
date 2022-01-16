/*******************************************************************************
 Copyright (C) 2016 Hangzhou Telin Technologies Co.,Ltd. All Rights Reserved.
 --------------------------------------------------------------------------------
 文件名称: list.h 
 功能描述: 
*******************************************************************************/
/*
 * Doubly-linked list
 * Copyright (c) 2009, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef LIST_H
#define LIST_H
#define NULL 0
//应该是一个双向链表
/**
 * struct dl_list - Doubly-linked list
 */
struct dl_list {
    struct dl_list *next;
    struct dl_list *prev;
};

#define DL_LIST_HEAD_INIT(l) { &(l), &(l) }

static inline void dl_list_init(struct dl_list *list)
{
    list->next = list;
    list->prev = list;
}
//头插法，后进先出
//实现的核心都是再list之后插入
static inline void dl_list_add(struct dl_list *list, struct dl_list *item)
{
    item->next = list->next;
    item->prev = list;
    list->next->prev = item;
    list->next = item;
}
//尾插法，先进先出
static inline void dl_list_add_tail(struct dl_list *list, struct dl_list *item)
{
    dl_list_add(list->prev, item);
}
static inline  void __list_del(struct dl_list *prev, struct dl_list *next)
{
    next->prev = prev;
    prev->next = next;
}
static inline void dl_list_del(struct dl_list *item)
{
#if  0
    item->next->prev = item->prev;
    item->prev->next = item->next;
#else
    __list_del(item->prev,item->next);
#endif
    item->next = NULL;
    item->prev = NULL;
}

static inline int dl_list_empty(struct dl_list *list)
{
    return list->next == list;
}

static inline unsigned int dl_list_len(struct dl_list *list)
{
    struct dl_list *item;
    int count = 0;
    for (item = list->next; item != list; item = item->next)
        count++;
    return (unsigned int)count;
}

/**
 * list_replace - replace old entry by new one
 * @old : the element to be replaced
 * @new : the new element to insert
 *
 * If @old was empty, it will be overwritten.
 */
static inline void dl_list_replace(struct dl_list *old,
    struct dl_list *new)
{
    new->next = old->next;
    new->next->prev = new;
    new->prev = old->prev;
    new->prev->next = new;
}
// dl_list_replace_init与dl_list_replace不同之处在于，list_replace_init会将旧的节点重新初始化，让前驱和后继指向自己。
static inline void dl_list_replace_init(struct dl_list *old,
     struct dl_list *new)
{
    dl_list_replace(old, new);
    dl_list_init(old);//重新初始化
}

/**dl_list_move
 * 函数实现的功能是删除item指向的节点，同时将其以头插法插入到head中
 * 用于提升某一个元素的等级
 * dl_list_move - delete from one list and add as another's head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
static inline void dl_list_move(struct dl_list *item, struct dl_list *head)
{
    __list_del(item->prev,item->next);
    dl_list_add(head,item);
}

/**
 * list_move_tail - delete from one list and add as another's tail
 * @list: the entry to move
 * @head: the head that will follow our entry
 */
static inline void dl_list_move_tail(struct dl_list *item,
      struct dl_list *head)
{
    __list_del(item->prev,item->next);
    dl_list_add_tail(head,item);
}

static inline void __dl_list_splice(struct dl_list *list,
     struct dl_list *head)
{
 struct dl_list *first = list->next;
 struct dl_list *last = list->prev;
 struct dl_list *at = head->next;

 first->prev = head;
 head->next = first;

 last->next = at;
 at->prev = last;
}

/**
 * list_splice - join two lists
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static inline void dl_list_splice(struct dl_list *list, struct dl_list *head)
{
 if (!dl_list_empty(list))
  __dl_list_splice(list, head);
}

/**
 * list_splice_init - join two lists and reinitialise the emptied list.
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * The list at @list is reinitialised
 */
static inline void dl_list_splice_init(struct dl_list *list,
        struct dl_list *head)
{
 if (!dl_list_empty(list)) {
  __dl_list_splice(list, head);
  dl_list_init(list);//置空
 }
}

#ifndef offsetof
#define offsetof(type, member) ((long) &((type *) 0)->member)
#endif
// 得到节点结构体的地址，得到地址后就可以对结构体中的元素进行操作了。
// 依靠list_entry(ptr, type, member)函数，内核链表的增删查改都不需要知道list_head结构体所嵌入式的对象，就可以完成各种操作。
#define dl_list_entry(item, type, member) \
    ((type *) ((char *) item - offsetof(type, member)))

#define dl_list_first(list, type, member) \
    (dl_list_empty((list)) ? NULL : \
     dl_list_entry((list)->next, type, member))

#define dl_list_last(list, type, member) \
    (dl_list_empty((list)) ? NULL : \
     dl_list_entry((list)->prev, type, member))

#define dl_list_for_each(item, list, type, member) \
    for (item = dl_list_entry((list)->next, type, member); \
         &item->member != (list); \
         item = dl_list_entry(item->member.next, type, member))
// 这函数是为了避免在遍历链表的过程中因pos节点被释放而造成的断链。
// 这个时候就要求我们另外提供一个与pos同类型的指针n，在for循环中暂存pos下一个节点的地址。
#define dl_list_for_each_safe(item, n, list, type, member) \
    for (item = dl_list_entry((list)->next, type, member), \
             n = dl_list_entry(item->member.next, type, member); \
         &item->member != (list); \
         item = n, n = dl_list_entry(n->member.next, type, member))
//反向便利链表
#define dl_list_for_each_reverse(item, list, type, member) \
    for (item = dl_list_entry((list)->prev, type, member); \
         &item->member != (list); \
         item = dl_list_entry(item->member.prev, type, member))

#define DEFINE_DL_LIST(name) \
    struct dl_list name = { &(name), &(name) }


#endif /* LIST_H */
