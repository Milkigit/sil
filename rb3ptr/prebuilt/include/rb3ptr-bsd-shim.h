#define RB_HELPERS(NAME, TYPE, FIELD) \
        static inline struct rb3_head *NAME##_RB_get_head(struct TYPE *container) { return &container->FIELD; } \
        static inline struct TYPE *NAME##_RB_get_container(struct rb3_head *head) { return (struct TYPE *)((char *) head - offsetof(struct TYPE, FIELD)); } \

#define RB_PROTOTYPE(NAME, TYPE, FIELD, CMP) \
        RB_HELPERS(NAME, TYPE, FIELD) \
        RB3_GEN_INLINE_PROTO(NAME, struct TYPE, NAME##_RB_get_head, NAME##_RB_get_container) \
        RB3_GEN_NODECMP_PROTO(NAME, /* no suffix */, struct TYPE, NAME##_RB_get_head, NAME##_RB_get_container, CMP)

#define RB_PROTOTYPE_STATIC(NAME, TYPE, FIELD, CMP) \
        RB_HELPERS(NAME, TYPE, FIELD) \
        RB3_GEN_INLINE_PROTO_STATIC(NAME, struct TYPE, NAME##_RB_get_head, NAME##_RB_get_container) \
        RB3_GEN_NODECMP_PROTO_STATIC(NAME, /* no suffix */, struct TYPE, NAME##_RB_get_head, NAME##_RB_get_container, CMP)

#define RB_GENERATE(NAME, TYPE, FIELD, CMP) \
        RB3_GEN_NODECMP(NAME, /* no suffix */, struct TYPE, NAME##_RB_get_head, NAME##_RB_get_container, CMP)

#define RB_GENERATE_STATIC(NAME, TYPE, FIELD, CMP) \
        RB3_GEN_NODECMP_STATIC(NAME, /* no suffix */, struct TYPE, NAME##_RB_get_head, NAME##_RB_get_container, CMP)


/* Our link head doesn't point to the containing struct, so we can dismiss the
 * argument... */
#define RB_ENTRY(structname) \
        struct rb3_head

#define RB_HEAD(NAME, TYPE) \
        RB3_GEN_TREE_DEFINITION(NAME)

#define RB_INIT(t) \
        rb3_init(&(t)->tree)

#define RB_EXIT(t) \
        rb3_exit(&(t)->tree)

#define RB_INSERT(NAME, tree, elm) \
        NAME##_insert(tree, elm)

#define RB_FIND(NAME, tree, elm) \
        NAME##_find(tree, elm)

#define RB_REMOVE(NAME, tree, elm) \
        NAME##_delete_head(elm)

#define RB_MIN(NAME, tree) \
        NAME##_get_min(tree)

#define RB_MAX(NAME, tree) \
        NAME##_get_max(tree)

/* `tree` is unused even in tree.h */
#define RB_PREV(NAME, tree, elm) \
        NAME##_get_prev(elm)

/* `tree` is unused even in tree.h */
#define RB_NEXT(NAME, tree, elm) \
        NAME##_get_next(elm)
