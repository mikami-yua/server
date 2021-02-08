#pragma once
#ifndef LIST_H
#define LIST_H
//#include "msg_type.h"
/*
链表管理
*/
struct list_head {
	struct list_head* next, * prev;

};



//宏
#define LIST_POSITION ((void *)0x0)

#define LIST_HEAD_INIT(name) {&(name),&name}
#define LIST_HEAD(name)\
	struct list_head name=LIST_HEAD_INIT(name)
#define INIT_LIST_HEAD(ptr)\
	do{(ptr)->next=(ptr);(ptr)->prev=(ptr);\
	}while(0)

#define list_for_each(pos,head)\
	for(pos=(head)->next;pos!=head;pos=pos->next)//对链表从前向后遍历

#define list_for_each_prev(pos,head)\
	for(pos=(head)->prev;pos!=head;pos=pos->prev)//链表从后向前遍历

#define offset_of(t,m) ((size_t)(&((t *)0)->m))//通过链表遍历访问的是list_head的地址，希望通过指针偏移，把指针移动到client_friend的地址  size_t：32位下是无符号int   64位是无符号long

#define containter_of(ptr,type,member)\
	((char *)(ptr))-offset_of(type,member)

#define list_entry(ptr,type,member)\
	containter_of(ptr,type,member)



void _list_add(struct list_head* node, struct list_head* prev, struct list_head* next);
void _list_del(struct list_head* prev, struct list_head* next);
void list_add(struct list_head* node, struct list_head* head);
void list_del(struct list_head* entry);
void list_del_init(struct list_head* entry);
void list_add_tail(struct list_head* node, struct list_head* head);
void list_empty(const struct list_head* head);




#endif // !LIST_H

