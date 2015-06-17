#ifndef PORTALS_RBTREE_H
#define PORTALS_RBTREE_H
#include <stdio.h>
#include <stdlib.h>
struct rb_node
{
	struct rb_node *rb_parent;
	int rb_color;
#define RB_RED     0
#define RB_BLACK   1
	struct rb_node *rb_right;
	struct rb_node *rb_left;
};

#define container_of(ptr, type, member) ( { \
        const typeof( ((type *)0)->member ) *__mptr = (ptr); \
        (type *)( (char *)__mptr - offsetof(type,member) ); } )


struct rb_root
{
	struct rb_node *rb_node;
};

//#define RB_ROOT(struct rb_root) {NULL,}
#define rb_entry(ptr,type,member) container_of(ptr, type, member)

#ifdef __cplusplus
extern "C"
{
#endif

	void rb_insert_color(struct rb_node *, struct rb_root *);
	void rb_erase(struct rb_node *, struct rb_root *);

	/* Find logical next and previous nodes in a tree */
	struct rb_node *rb_next(struct rb_node *);
	struct rb_node *rb_prev(struct rb_node *);
	struct rb_node *rb_first(struct rb_root *);
	struct rb_node *rb_last(struct rb_root *);

	/* Fast replacement of a single node without remove/rebalance/add/rebalance */
	void rb_replace_node(struct rb_node *victim, struct rb_node *n,
	struct rb_root *root);

#ifdef __cplusplus
}
#endif

static inline void rb_link_node(struct rb_node *node, struct rb_node *parent,
struct rb_node **rb_link)
{
	node->rb_parent = parent;
	node->rb_color = RB_RED;
	node->rb_left = node->rb_right = NULL;

	*rb_link = node;
}

#endif
