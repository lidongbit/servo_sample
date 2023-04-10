#ifndef CIRCLE_DUL_LIST_H_
#define CIRCLE_DUL_LIST_H_
/*单链表*/
#ifdef __cplusplus
extern "C" {
#endif

typedef struct list_node{
	char *pdata;
	struct list_node *pprev;
	struct list_node *pnext;
}dlist_node_t;

extern dlist_node_t *circle_dlist_init(void);

extern dlist_node_t *circle_dlist_tail_insert(dlist_node_t *phead, char *pdata);

extern int circle_dlist_get_nums(dlist_node_t *phead);

extern char *circle_dlist_head_eat(dlist_node_t* phead);

extern char *circle_dlist_tail_eat(dlist_node_t* phead);

extern void circle_dlist_end(dlist_node_t* phead);

extern void circle_dlist_print(dlist_node_t* phead);

#ifdef __cplusplus
}
#endif

#endif
