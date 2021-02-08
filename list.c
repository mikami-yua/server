#include"list.h"
#include"msg_type.h"
/*
	插入一个新节点
	@node：节点
	@prev：指向上一个节点的指针
	@next：指向下一个节点的指针
*/
void _list_add(struct list_head* node, struct list_head* prev, struct list_head* next) {
	next->prev = node;
	node->next = next;
	node->prev = prev;
	prev->next = node;
}

void _list_del(struct list_head* prev, struct list_head* next) {
	next->prev = prev;
	prev->next = next;
}


//头叉
void list_add(struct list_head* node, struct list_head* head) {
	_list_add(node, head, head->next);
}
//尾插
void list_add_tail(struct list_head* node, struct list_head* head) {
	_list_add(node, head->prev, head);
}

//删除
void list_del(struct list_head* entry) {
	_list_del(entry->prev, entry->next);
	entry->next = LIST_POSITION;
	entry->prev = LIST_POSITION;//清空指针
}
void list_del_init(struct list_head* entry) {
	_list_del(entry->prev, entry->next);
	INIT_LIST_HEAD(entry);
}

//链表是否为空
void list_empty(const struct list_head* head) {
	return head->next == head;
}

//p17