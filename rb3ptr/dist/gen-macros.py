#!/usr/bin/python3

"""Build macro-generation headers for to wrap the type-generic RB3 API.

You could say this is a program that writes a program that writes a program...
"""

import sys


def cs(xs):
    return ', '.join(xs)

params0 = 'OUTER_HEAD_TYPE OUTER_TREE_TYPE NODE_TYPE HEAD_FROM_NODE NODE_FROM_HEAD'.split()
args0 = ['struct BASENAME##_head', 'struct BASENAME', 'NODE_TYPE', 'HEAD_FROM_NODE', 'NODE_FROM_HEAD']

params1 = 'GET_HEAD GET_NODE INNER_HEAD OUTER_HEAD INNER_TREE OUTER_TREE'.split()
args1 = 'BASENAME##_get_head BASENAME##_get_node BASENAME##_inner_head BASENAME##_outer_head BASENAME##_inner_tree BASENAME##_outer_tree'.split()

params2 = 'init exit isempty get_min get_max get_prev get_next get_root has_child get_child get_parent get_prev_ancestor get_next_ancestor get_prev_descendant get_next_descendant insert_below delete_head get_parent_dir get_base get_containing_tree'.split()
args2 = ['BASENAME##_{}'.format(name) for name in params2]

params3 = 'nodecmp find_in_subtree find_parent_in_subtree delete_in_subtree insert_in_subtree find delete insert'.split()
args3 = ['BASENAME##_{}##SUFFIX'.format(name) for name in params3]


proxies = """
/*
 * This is the stuff the user wants to use
 */
#define RB3_GEN_STRUCTS(BASENAME)  \\
    RB3_GEN_STRUCTS_REAL(BASENAME##_head, BASENAME)

#define RB3_GEN_INLINE(BASENAME, NODE_TYPE, HEAD_FROM_NODE, NODE_FROM_HEAD)  \\
    RB3_GEN_INLINE_REAL({args0}, {args1}, {args2})
#define RB3_GEN_INLINE_PROTO(BASENAME, NODE_TYPE, HEAD_FROM_NODE, NODE_FROM_HEAD)  \\
    RB3_GEN_INLINE_PROTO_REAL({args0}, {args1}, {args2})

#define RB3_GEN_NODECMP(BASENAME, SUFFIX, NODE_TYPE, HEAD_FROM_NODE, NODE_FROM_HEAD, COMPARE_NODE)  \\
    RB3_GEN_NODECMP_REAL({args0}, COMPARE_NODE, {args1}, {args2}, {args3})
#define RB3_GEN_NODECMP_PROTO(BASENAME, SUFFIX, NODE_TYPE, HEAD_FROM_NODE, NODE_FROM_HEAD, COMPARE_NODE)  \\
    RB3_GEN_NODECMP_PROTO_REAL({args0}, COMPARE_NODE, {args1}, {args2}, {args3})


#define RB3_FOREACH(BASENAME, TREE, NODE) \\
    for (NODE = BASENAME##_get_min(TREE); \\
        !!NODE; NODE = BASENAME##_get_next(NODE))

#define RB3_FOREACH_SAFE(BASENAME, TREE, NODE, NODE1) \\
    for (NODE = BASENAME##_get_min(TREE); \\
        (!!NODE ? (NODE1 = BASENAME##_get_next(NODE), !!NODE) : !!NODE); \\
        NODE = NODE1)

/* (END stuff) */
""".format(args0=cs(args0), args1=cs(args1), args2=cs(args2), args3=cs(args3))

things = [
    ('templates/rb3ptr-structs.tpl',
    '#define RB3_GEN_STRUCTS_REAL(OUTER_HEAD_STRUCT_NAME, OUTER_TREE_STRUCT_NAME)  \\\n'),
    ('templates/rb3ptr-inline-proto.tpl',
    '#define RB3_GEN_INLINE_PROTO_REAL({}, {}, {})  \\\n'.format(cs(params0), cs(params1), cs(params2))),
    ('templates/rb3ptr-nodecmp-proto.tpl',
    '#define RB3_GEN_NODECMP_PROTO_REAL({}, COMPARE_NODE, {}, {}, {})  \\\n'.format(cs(params0), cs(params1), cs(params2), cs(params3))),
    ('templates/rb3ptr-inline.tpl',
    '#define RB3_GEN_INLINE_REAL({}, {}, {})  \\\n'.format(cs(params0), cs(params1), cs(params2))),
    ('templates/rb3ptr-nodecmp.tpl',
    '#define RB3_GEN_NODECMP_REAL({}, COMPARE_NODE, {}, {}, {})  \\\n'.format(cs(params0), cs(params1), cs(params2), cs(params3))),
]


def clean_escapes(lines):
    maxlen = max(len(line) for line in lines)
    out = []
    i = 0
    for line in lines[:-1]:
        line = line.rstrip(' \\\n')
        out.append(line + ' '*(2 + maxlen - len(line)) + '\\\n')
    out.append(lines[-1])
    return out

def make_macro(filepath, firstline):
    lines = open(filepath).readlines()
    out = [firstline]
    out += clean_escapes(lines)
    return ''.join(out)

def cat(filepath):
    print(open(filepath).read())


print('/*')
cat('LICENSE.txt')
print('*/')
print()
print(r"""
/*
 * WARNING:
 *
 * This file was autogenerated with gen-macros.py from files under templates/
 *
 */
""")

print("""
#ifndef RB3_GEN_HEADER
#define RB3_GEN_HEADER
""")


print(proxies)

for filepath, firstline in things:
    print(make_macro(filepath, firstline))


print()
print('#endif  /* RB3_GEN_HEADER */')
