
#ifndef _GENLIST_H_
#define _GENLIST_H_

#include <stdio.h>
#include <stdlib.h>

typedef struct _node
{
    struct _node *next;
    struct _node *prev;
}list_node, list_head;

#define INIT_LIST_HEAD(head) { &head, &head }
#define LIST_HEAD(head) list_head head = INIT_LIST_HEAD(head);

#define list_foreach(node, list) for(node = list->next; node != list; node = node->next)

static inline int list_empty(list_head *list)
{
	if ( list->next == list ) return 1;
	else			  return 0;
}
static inline void _list_add(list_node *node, list_node *prev, list_node *next)
{
    prev->next = node;
    next->prev = node;
    node->next = next;
    node->prev = prev;
}

static inline void list_add(list_node *node, list_node *new_node)
{
    _list_add(new_node, node->prev, node);
}

static inline void list_add_head(list_head *list, list_node *node)
{
    _list_add(node, list, list->next);
}

static inline void list_add_tail(list_head *list, list_node *node)
{
    _list_add(node, list->prev, list);
}

static inline list_node* list_pop_front(list_head *list)
{
	list_node *node = list->next;
	list->next = node->next;
	node->next->prev = list;
	return node;
}

static inline list_node* list_front( list_head *list )
{
	if ( list->next == list )
		return NULL;
	else
		return list->next;
}

static inline void list_remove(list_head *list, list_node *node)
{
    list_node *temp = NULL;
    list_foreach( temp, list ) {
        if ( temp == node ) {
            temp->next->prev = temp->prev;
            temp->prev->next = temp->next;
            break;
        }
    }
}

#endif /*_GENLIST_H_*/
