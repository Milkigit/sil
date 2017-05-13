#!/usr/bin/python3

"""Build macro-generation headers for to wrap the type-generic RB3 API.

You could say this is a program that writes a program that writes a program...
"""

import sys
import subprocess


def cs(xs):
    return ', '.join(xs)

def cleanescapes(lines):
    return [line.rstrip(' \\\n') + ' \\\n' for line in lines]

def filetomacro(name):
    return ''.join(cleanescapes(open('templates/' + name + '.tpl').readlines()))


params = 'RB3_API RB3_API_INLINE RB3_API_STATIC_INLINE RB3_COLD RB3_INLINE RB3_NEVERINLINE'.split()

params0 = 'OUTER_TREE_TYPE NODE_TYPE GET_HEAD GET_NODE'.split()
args0 = ['struct BASENAME', 'NODE_TYPE', 'GET_HEAD', 'GET_NODE']

params1 = 'INNER_TREE OUTER_TREE'.split()
args1 = 'BASENAME##_inner_tree BASENAME##_outer_tree'.split()

params2 = 'init exit isempty get_min get_max get_prev get_next get_root has_child get_child get_parent get_prev_ancestor get_next_ancestor get_prev_descendant get_next_descendant insert_below delete_head get_parent_dir get_base get_containing_tree'.split()
args2 = ['BASENAME##_{}'.format(name) for name in params2]

params3 = 'nodecmp find_in_subtree find_parent_in_subtree delete_in_subtree insert_in_subtree find delete insert'.split()
args3 = ['BASENAME##_{}##SUFFIX'.format(name) for name in params3]


proxies = """
/*
 * This is the stuff the user wants to use
 */

#define RB3_GEN_IMPL_HEADER()  \\
    RB3_GEN_IMPL_HEADER_REAL(RB3_EXTERN_ATTRS)

#define RB3_GEN_IMPL_HEADER_STATIC()  \\
    RB3_GEN_IMPL_HEADER_REAL(RB3_STATIC_ATTRS)

#define RB3_GEN_IMPL()  \\
    RB3_GEN_IMPL_REAL(RB3_EXTERN_ATTRS)

#define RB3_GEN_IMPL_STATIC()  \\
    RB3_GEN_IMPL_REAL(RB3_STATIC_ATTRS)


#define RB3_GEN_STRUCTS(BASENAME)  \\
    RB3_GEN_STRUCTS_REAL(BASENAME)


#define RB3_GEN_INLINE_PROTO(BASENAME, NODE_TYPE, GET_HEAD, GET_NODE)  \\
    RB3_GEN_INLINE_PROTO_REAL(RB3_EXTERN_ATTRS, {args0}, {args1}, {args2})

#define RB3_GEN_INLINE_PROTO_STATIC(BASENAME, NODE_TYPE, GET_HEAD, GET_NODE)  \\
    RB3_GEN_INLINE_PROTO_REAL(RB3_STATIC_ATTRS, {args0}, {args1}, {args2})

#define RB3_GEN_NODECMP(BASENAME, SUFFIX, NODE_TYPE, GET_HEAD, GET_NODE, COMPARE_NODE)  \\
    RB3_GEN_NODECMP_REAL(RB3_EXTERN_ATTRS, {args0}, COMPARE_NODE, {args1}, {args2}, {args3})

#define RB3_GEN_NODECMP_STATIC(BASENAME, SUFFIX, NODE_TYPE, GET_HEAD, GET_NODE, COMPARE_NODE)  \\
    RB3_GEN_NODECMP_REAL(RB3_STATIC_ATTRS, {args0}, COMPARE_NODE, {args1}, {args2}, {args3})

#define RB3_GEN_NODECMP_PROTO(BASENAME, SUFFIX, NODE_TYPE, GET_HEAD, GET_NODE, COMPARE_NODE)  \\
    RB3_GEN_NODECMP_PROTO_REAL(RB3_EXTERN_ATTRS, {args0}, COMPARE_NODE, {args1}, {args2}, {args3})

#define RB3_GEN_NODECMP_PROTO_STATIC(BASENAME, SUFFIX, NODE_TYPE, GET_HEAD, GET_NODE, COMPARE_NODE)  \\
    RB3_GEN_NODECMP_PROTO_REAL(RB3_STATIC_ATTRS, {args0}, COMPARE_NODE, {args1}, {args2}, {args3})


#define RB3_FOREACH(BASENAME, TREE, NODE) \\
    for (NODE = BASENAME##_get_min(TREE); \\
        !!NODE; NODE = BASENAME##_get_next(NODE))

#define RB3_FOREACH_SAFE(BASENAME, TREE, NODE, NODE1) \\
    for (NODE = BASENAME##_get_min(TREE); \\
        (!!NODE ? (NODE1 = BASENAME##_get_next(NODE), !!NODE) : !!NODE); \\
        NODE = NODE1)

/* (END stuff) */
""".format(args0=cs(args0), args1=cs(args1), args2=cs(args2), args3=cs(args3))


content = """
/*
 * ===========================================================================
 * SORRY FOR THIS MESS
 *
 * These macros are only for implementation. Not part of the API.
 * ===========================================================================
 */

#define RB3_GEN_STRUCTS_REAL(OUTER_TREE_STRUCT_NAME) RB3_GEN_STRUCTS_TOTALLY_REAL(OUTER_TREE_STRUCT_NAME)
#define RB3_GEN_INLINE_PROTO_REAL(argstoexpand, {params0}, {params1}, {params2}) RB3_GEN_INLINE_PROTO_TOTALLY_REAL(argstoexpand, {params0}, {params1}, {params2})
#define RB3_GEN_NODECMP_PROTO_REAL(argstoexpand, {params0}, COMPARE_NODE, {params1}, {params2}, {params3}) RB3_GEN_NODECMP_PROTO_TOTALLY_REAL(argstoexpand, {params0}, COMPARE_NODE, {params1}, {params2}, {params3})
#define RB3_GEN_NODECMP_REAL(argstoexpand, {params0}, COMPARE_NODE, {params1}, {params2}, {params3}) RB3_GEN_NODECMP_TOTALLY_REAL(argstoexpand, {params0}, COMPARE_NODE, {params1}, {params2}, {params3})

#define RB3_GEN_IMPL_HEADER_REAL(argstoexpand)  RB3_GEN_IMPL_HEADER_TOTALLY_REAL(argstoexpand)
#define RB3_GEN_IMPL_REAL(argstoexpand) RB3_GEN_IMPL_TOTALLY_REAL(argstoexpand)


#define RB3_GEN_STRUCTS_TOTALLY_REAL(OUTER_TREE_STRUCT_NAME)  \\
{tpl_structs}

#define RB3_GEN_INLINE_PROTO_TOTALLY_REAL({params}, {params0}, {params1}, {params2})  \\
{tpl_inline_proto}

#define RB3_GEN_NODECMP_PROTO_TOTALLY_REAL({params}, {params0}, COMPARE_NODE, {params1}, {params2}, {params3})  \\
{tpl_nodecmp_proto}

#define RB3_GEN_NODECMP_TOTALLY_REAL({params}, {params0}, COMPARE_NODE, {params1}, {params2}, {params3})  \\
{tpl_nodecmp}


#define RB3_GEN_IMPL_TOTALLY_REAL_TYPES({params})  \\
{tpl_types}

#define RB3_GEN_IMPL_TOTALLY_REAL_BASIC({params})  \\
{tpl_basic}

#define RB3_GEN_IMPL_TOTALLY_REAL_NAVIGATE({params})  \\
{tpl_navigate}

#define RB3_GEN_IMPL_TOTALLY_REAL_INTERNAL({params})  \\
{tpl_internal}

#define RB3_GEN_IMPL_TOTALLY_REAL_IMPL({params})  \\
{tpl_impl}

#define RB3_GEN_IMPL_HEADER_TOTALLY_REAL({params})  \\
    RB3_GEN_IMPL_TOTALLY_REAL_TYPES({params})  \\
    RB3_GEN_IMPL_TOTALLY_REAL_BASIC({params})  \\
    RB3_GEN_IMPL_TOTALLY_REAL_NAVIGATE({params})  \\
    RB3_GEN_IMPL_TOTALLY_REAL_INTERNAL({params})

#define RB3_GEN_IMPL_TOTALLY_REAL({params})  \\
    RB3_GEN_IMPL_HEADER_TOTALLY_REAL({params})  \\
    RB3_GEN_IMPL_TOTALLY_REAL_IMPL({params})

""".format(params=cs(params), params0=cs(params0), params1=cs(params1), params2=cs(params2), params3=cs(params3),
        tpl_structs=filetomacro('wrapper-structs'),
        tpl_inline_proto=filetomacro('wrapper-inline-proto'),
        tpl_nodecmp_proto=filetomacro('wrapper-nodecmp-proto'),
        tpl_nodecmp=filetomacro('wrapper-nodecmp'),
        tpl_types=filetomacro('types'),
        tpl_basic=filetomacro('basic'),
        tpl_navigate=filetomacro('navigate'),
        tpl_internal=filetomacro('internal'),
        tpl_impl=filetomacro('impl'))


def cat(filepath):
    print(open(filepath).read())

def shell(cmd):
    return subprocess.check_output(cmd, shell=True).decode()


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
 * Autogenerated from git commit {git_commit}
 */
""".format(git_commit=shell('git rev-parse HEAD').strip()))
print("""
#ifndef RB3_GEN_HEADER
#define RB3_GEN_HEADER
""")
cat('templates/defs.tpl')
print(proxies)
print(content)
print()
print('#endif  /* RB3_GEN_HEADER */')
