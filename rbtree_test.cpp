/**
 * @file    rbtree_test.cpp
 * <pre>
 * Copyright (c) 2019, Gaaagaa All rights reserved.
 * 
 * 文件名称：rbtree_test.cpp
 * 创建日期：2019年09月12日
 * 文件标识：
 * 文件摘要：红黑树的接口测试程序。
 * 
 * 当前版本：1.0.0.0
 * 作    者：
 * 完成日期：2019年09月12日
 * 版本摘要：
 * 
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 * 版本摘要：
 * </pre>
 */

#include "xrbtree.h"

#include <stdio.h>
#include <memory.h>
#include <set>
#include <chrono>
#include <memory>

////////////////////////////////////////////////////////////////////////////////

using xtime_clock = std::chrono::system_clock;
using xtime_point = std::chrono::system_clock::time_point;
using xtime_value = std::chrono::microseconds;

#define xtime_dcast  std::chrono::duration_cast< xtime_value >

XRBTREE_TYPE_API(int)

long long xalloc_count = 0;

xrbt_void_t * xalloc_byte_xfunc_alloc(xrbt_vkey_t xrbt_vkey, xrbt_size_t xst_nsize)
{
    xalloc_count += 1;
    return malloc(xst_nsize);
}

xrbt_void_t xalloc_byte_xfunc_dealloc(xrbt_void_t * xrbt_node, xrbt_size_t xst_nsize)
{
    xalloc_count -= 1;
    free(xrbt_node);
}

void test_xrbtree(int max_insert)
{
    xtime_point xtm_begin;
    xtime_value xtm_value;

    long long testvalue  = 0;

    //======================================

    xrbt_allocator_t xtree_alloc = xrbtree_default_allocator_int();
    xtree_alloc.xfunc_alloc   = &xalloc_byte_xfunc_alloc;
    xtree_alloc.xfunc_dealloc = &xalloc_byte_xfunc_dealloc;

    x_rbtree_ptr xtree_ptr = xrbtree_create_int(XRBT_NULL, &xtree_alloc);

    // insert
    xtm_begin = xtime_clock::now();
    for (int i = 1; i <= max_insert; ++i)
    {
        xrbtree_insert_int(xtree_ptr, i, XRBT_NULL);
    }
    xtm_value = xtime_dcast(xtime_clock::now() - xtm_begin);
    printf("[ C ] insert   time cost: %8d\n", (int)xtm_value.count());
    printf("[ C ] current  tree size: %8d\n", (int)xrbtree_size(xtree_ptr));
    printf("[ C ] allocator    count: %8d\n", (int)xalloc_count);

    // erase
    testvalue = 0;
    xtm_begin = xtime_clock::now();
    for (int i = 1; i <= max_insert; i += 100)
    {
        for (int j = i; j < (i + 10); ++j)
        {
            if (xrbtree_erase_int(xtree_ptr, j))
                testvalue += 1;
        }
    }
    xtm_value = xtime_dcast(xtime_clock::now() - xtm_begin);
    printf("[ C ] erase    time cost: %8d ==> count: %lld\n", (int)xtm_value.count(), testvalue);
    printf("[ C ] current  tree size: %8d\n", (int)xrbtree_size(xtree_ptr));
    printf("[ C ] allocator    count: %8d\n", (int)xalloc_count);

    // find
    testvalue = 0;
    xtm_begin = xtime_clock::now();
    for (int i = 1; i <= max_insert; i += 1)
    {
        x_rbnode_iter xiter = xrbtree_find_int(xtree_ptr, i);
        if (xiter != xrbtree_end(xtree_ptr))
            testvalue += xrbtree_ikey_int(xiter);
    }
    xtm_value = xtime_dcast(xtime_clock::now() - xtm_begin);
    printf("[ C ] find     time cost: %8d ==> sum  : %lld\n", (int)xtm_value.count(), testvalue);

    // iterator
    testvalue = 0;
    xtm_begin = xtime_clock::now();
    for (x_rbnode_iter xiter = xrbtree_begin(xtree_ptr);
         xiter != xrbtree_end(xtree_ptr);
         xiter = xrbtree_next(xiter))
    {
        testvalue += xrbtree_ikey_int(xiter);
    }
    xtm_value = xtime_dcast(xtime_clock::now() - xtm_begin);
    printf("[ C ] iterator time cost: %8d ==> sum  : %lld\n", (int)xtm_value.count(), testvalue);

    // clear
    xtm_begin = xtime_clock::now();
    xrbtree_clear(xtree_ptr);
    xtm_value = xtime_dcast(xtime_clock::now() - xtm_begin);
    printf("[ C ] clear    time cost: %8d\n", (int)xtm_value.count());
    printf("[ C ] current  tree size: %8d\n", (int)xrbtree_size(xtree_ptr));

    xrbtree_destroy(xtree_ptr);
    xtree_ptr = XRBT_NULL;

    printf("[ C ] allocator    count: %8d\n", (int)xalloc_count);

    //======================================
}

void test_settree(int max_insert)
{
    xtime_point xtm_begin;
    xtime_value xtm_value;

    long long testvalue  = 0;

    //======================================

    std::set< int > xset_tree;

    // insert 
    xtm_begin = xtime_clock::now();
    for (int i = 1; i <= max_insert; ++i)
    {
        xset_tree.insert(i);
    }
    xtm_value = xtime_dcast(xtime_clock::now() - xtm_begin);
    printf("[CPP] insert   time cost: %8d\n", (int)xtm_value.count());
    printf("[CPP] current  tree size: %8d\n", (int)xset_tree.size());

    // erase
    testvalue = 0;
    xtm_begin = xtime_clock::now();
    for (int i = 1; i <= max_insert; i += 100)
    {
        for (int j = i; j < (i + 10); ++j)
        {
            if (xset_tree.erase(j) > 0)
                testvalue += 1;
        }
    }
    xtm_value = xtime_dcast(xtime_clock::now() - xtm_begin);
    printf("[CPP] erase    time cost: %8d ==> count: %lld\n", (int)xtm_value.count(), testvalue);
    printf("[CPP] current  tree size: %8d\n", (int)xset_tree.size());

    // find
    testvalue = 0;
    xtm_begin = xtime_clock::now();
    for (int i = 1; i <= max_insert; i += 1)
    {
        std::set< int >::iterator xiter = xset_tree.find(i);
        if (xiter != xset_tree.end())
            testvalue += *xiter;
    }
    xtm_value = xtime_dcast(xtime_clock::now() - xtm_begin);
    printf("[CPP] find     time cost: %8d ==> sum  : %lld\n", (int)xtm_value.count(), testvalue);

    // iterator
    testvalue = 0;
    xtm_begin = xtime_clock::now();
    for (std::set< int >::iterator xiter = xset_tree.begin();
         xiter != xset_tree.end();
         ++xiter)
    {
        testvalue += *xiter;
    }
    xtm_value = xtime_dcast(xtime_clock::now() - xtm_begin);
    printf("[CPP] iterator time cost: %8d ==> sum  : %lld\n", (int)xtm_value.count(), testvalue);

    // clear
    xtm_begin = xtime_clock::now();
    xset_tree.clear();
    xtm_value = xtime_dcast(xtime_clock::now() - xtm_begin);
    printf("[CPP] clear    time cost: %8d\n", (int)xtm_value.count());
    printf("[CPP] current  tree size: %8d\n", (int)xset_tree.size());

    //======================================
}

int main(int argc, char * argv[])
{
    int max_insert = 1000000;
    int first_test = 1;

    printf("Usage: %s < max_insert > < first_test_c : 0 or 1 >\n", argv[0]);

    if (argc >= 2) max_insert = std::atoi(argv[1]);
    if (argc >= 3) first_test = std::atoi(argv[2]);

    printf("Test insert : %d\n", max_insert);
    printf("//======================================\n");

    if (0 != first_test)
    {
        test_xrbtree(max_insert);
        test_settree(max_insert);
    }
    else
    {
        test_settree(max_insert);
        test_xrbtree(max_insert);
    }

    printf("//======================================\n");

    return 0;
}

