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
    return ''.join(cleanescapes(open('templates/' + name + '.c').readlines()))


params0 = 'OUTER_TREE_TYPE NODE_TYPE GET_HEAD GET_NODE'.split()
args0 = ['struct BASENAME', 'NODE_TYPE', 'GET_HEAD', 'GET_NODE']

params1 = 'INNER_TREE OUTER_TREE'.split()
args1 = 'BASENAME##_inner_tree BASENAME##_outer_tree'.split()

params2 = 'reset_tree isempty get_min get_max get_prev get_next get_minmax get_prevnext get_root has_child get_child get_parent get_prev_ancestor get_next_ancestor get_prev_descendant get_next_descendant link_and_rebalance unlink_and_rebalance get_parent_dir get_base get_containing_tree'.split()
args2 = ['BASENAME##_{}'.format(name) for name in params2]

params3 = 'nodecmp find_in_subtree find_parent_in_subtree delete_in_subtree insert_in_subtree find delete insert'.split()
args3 = ['BASENAME##_{}##SUFFIX'.format(name) for name in params3]


proxies = """
/*
 * This is the stuff the user wants to use
 */

#define RB3_GEN_IMPL_HEADER()  \\
    _RB3_GEN_IMPL_HEADER_REAL(_RB3_API_EXTERNIMPL)

#define RB3_GEN_IMPL_HEADER_STATIC()  \\
    _RB3_GEN_IMPL_HEADER_REAL(_RB3_API_STATICIMPL)

#define RB3_GEN_IMPL()  \\
    _RB3_GEN_IMPL_REAL(_RB3_API_EXTERNIMPL)

#define RB3_GEN_IMPL_STATIC()  \\
    _RB3_GEN_IMPL_REAL(_RB3_API_STATICIMPL)


#define RB3_GEN_INLINE_PROTO(BASENAME, NODE_TYPE, GET_HEAD, GET_NODE)  \\
    _RB3_GEN_INLINE_PROTO_REAL(_RB3_API_EXTERNIMPL, {args0}, {args1}, {args2})

#define RB3_GEN_INLINE_PROTO_STATIC(BASENAME, NODE_TYPE, GET_HEAD, GET_NODE)  \\
    _RB3_GEN_INLINE_PROTO_REAL(_RB3_API_STATICIMPL, {args0}, {args1}, {args2})

#define RB3_GEN_NODECMP(BASENAME, SUFFIX, NODE_TYPE, GET_HEAD, GET_NODE, COMPARE_NODE)  \\
    _RB3_GEN_NODECMP_REAL(_RB3_API_EXTERNIMPL, {args0}, COMPARE_NODE, {args1}, {args2}, {args3})

#define RB3_GEN_NODECMP_STATIC(BASENAME, SUFFIX, NODE_TYPE, GET_HEAD, GET_NODE, COMPARE_NODE)  \\
    _RB3_GEN_NODECMP_REAL(_RB3_API_STATICIMPL, {args0}, COMPARE_NODE, {args1}, {args2}, {args3})

#define RB3_GEN_NODECMP_PROTO(BASENAME, SUFFIX, NODE_TYPE, GET_HEAD, GET_NODE, COMPARE_NODE)  \\
    _RB3_GEN_NODECMP_PROTO_REAL(_RB3_API_EXTERNIMPL, {args0}, COMPARE_NODE, {args1}, {args2}, {args3})

#define RB3_GEN_NODECMP_PROTO_STATIC(BASENAME, SUFFIX, NODE_TYPE, GET_HEAD, GET_NODE, COMPARE_NODE)  \\
    _RB3_GEN_NODECMP_PROTO_REAL(_RB3_API_STATICIMPL, {args0}, COMPARE_NODE, {args1}, {args2}, {args3})

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

#define _RB3_GEN_INLINE_PROTO_REAL(_RB3_API, {params0}, {params1}, {params2})  \\
{tpl_inline_proto}

#define _RB3_GEN_NODECMP_PROTO_REAL(_RB3_API, {params0}, COMPARE_NODE, {params1}, {params2}, {params3})  \\
{tpl_nodecmp_proto}

#define _RB3_GEN_NODECMP_REAL(_RB3_API, {params0}, COMPARE_NODE, {params1}, {params2}, {params3})  \\
{tpl_nodecmp}

#define _RB3_GEN_IMPL_REAL_TYPES(_RB3_API)  \\
{tpl_types}

#define _RB3_GEN_IMPL_REAL_INTERNAL(_RB3_API)  \\
{tpl_internal}

#define _RB3_GEN_IMPL_REAL_NAVIGATE(_RB3_API)  \\
{tpl_navigate}

#define _RB3_GEN_IMPL_REAL_BASIC(_RB3_API)  \\
{tpl_basic}

#define _RB3_GEN_IMPL_REAL_IMPL(_RB3_API)  \\
{tpl_impl}

#define _RB3_GEN_IMPL_HEADER_REAL(_RB3_API)  \\
    _RB3_GEN_IMPL_REAL_TYPES(_RB3_API)  \\
    _RB3_GEN_IMPL_REAL_INTERNAL(_RB3_API)  \\
    _RB3_GEN_IMPL_REAL_NAVIGATE(_RB3_API)  \\
    _RB3_GEN_IMPL_REAL_BASIC(_RB3_API)

#define _RB3_GEN_IMPL_REAL(_RB3_API)  \\
    _RB3_GEN_IMPL_HEADER_REAL(_RB3_API)  \\
    _RB3_GEN_IMPL_REAL_IMPL(_RB3_API)

""".format(params0=cs(params0), params1=cs(params1), params2=cs(params2), params3=cs(params3),
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
cat('templates/defs.c')
print(proxies)
print(content)
cat('templates/wrapper-defs.c')
print()
print('#endif  /* RB3_GEN_HEADER */')
