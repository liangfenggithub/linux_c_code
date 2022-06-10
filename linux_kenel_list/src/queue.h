/*  queue.h  */

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdbool.h>
#include "linux_kenel_list.h"

#define get_queue_head(pos, head, member)        \
        list_entry((head)->next, typeof(*pos), member)

void queue_creat(struct list_head *list);
void in_queue(struct list_head *new, struct list_head *head);
void out_queue(struct list_head *entry);
int get_queue_size(struct list_head *head);
bool is_empt_queue(struct list_head *head);


#endif /* _QUEUE_H_ */