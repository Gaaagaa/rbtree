﻿/**
 * @file    xrbtree.h
 * <pre>
 * Copyright (c) 2019, Gaaagaa All rights reserved.
 * 
 * 文件名称：xrbtree.h
 * 创建日期：2019年09月09日
 * 文件标识：
 * 文件摘要：红黑树的相关数据定义以及操作接口。
 * 
 * 当前版本：1.0.0.0
 * 作    者：
 * 完成日期：2019年09月09日
 * 版本摘要：
 * 
 * 历史版本：
 * 原作者  ：
 * 完成日期：
 * 版本摘要：
 * </pre>
 */

#ifndef __XRBTREE_H__
#define __XRBTREE_H__

////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// 红黑树的相关数据定义以及操作接口

//====================================================================

// 
// 红黑树的相关数据定义
// 

#define XRBT_FALSE  0
#define XRBT_TRUE   1
#define XRBT_NULL   0

typedef void           xrbt_void_t;
typedef unsigned char  xrbt_byte_t;
typedef int            xrbt_int32_t;
typedef unsigned int   xrbt_uint32_t;
typedef unsigned int   xrbt_size_t;
typedef unsigned int   xrbt_bool_t;
typedef void *         xrbt_vkey_t;

/** 声明红黑树所使用的节点结构体 */
struct x_rbtree_node_t;

/** 声明红黑树所使用的节点迭代器 */
typedef struct x_rbtree_node_t * x_rbnode_iter;

/** 声明红黑树结构体 */
struct x_rbtree_t;

/** 声明红黑树对象指针 */
typedef struct x_rbtree_t * x_rbtree_ptr;

/**
 * @brief 比较节点索引键值的回调接口函数类型。
 * 
 * @param [in ] xrbt_lkey : 比较操作的左值。
 * @param [in ] xrbt_rkey : 比较操作的右值。
 * @param [out] xrbt_size : xrbt_lkey（or xrbt_rkey） 缓存大小。
 * 
 * @return xrbt_bool_t
 *         - 若 xrbt_lkey < xrbt_rkey ，返回 XRBT_TRUE；
 *         - 否则 返回 XRBT_FALSE。
 */
typedef xrbt_bool_t (* xfunc_vkey_cmp_t)(xrbt_vkey_t xrbt_lkey,
                                         xrbt_vkey_t xrbt_rkey,
                                         xrbt_size_t xrbt_size);

/**
 * @struct xrbt_allocator_t
 * @brief  红黑树所使用的 节点缓存分配器 结构体描述信息。
 */
typedef struct x_rbtree_node_allocator_t
{
    /**
     * @brief 申请节点对象缓存的回调接口（可以设置为 XRBT_NULL）。
     * @note  创建 x_rbtree_t 对象时，若该参数为 XRBT_NULL，则取内部默认的操作接口。
     * 
     * @param [in ] xrbt_vkey : 请求申请缓存的节点索引键（插入操作时回调回来的索引键）。
     * @param [in ] xst_nsize : 节点对象所需缓存的大小（即 请求申请缓存的大小）。
     */
    xrbt_void_t * (* xfunc_alloc)(xrbt_vkey_t xrbt_vkey, xrbt_size_t xst_nsize);

    /**
     * @brief 释放节点对象缓存的回调接口（可以设置为 XRBT_NULL）。
     * @note  创建 x_rbtree_t 对象时，若该参数为 XRBT_NULL，则取内部默认的操作接口。
     * 
     * @param [in ] xrbt_node : 待释放的节点对象缓存。
     * @param [in ] xst_nsize : 节点对象缓存的大小。
     */
    xrbt_void_t (* xfunc_dealloc)(xrbt_void_t * xrbt_node, xrbt_size_t xst_nsize);

    /**
     * @brief 拷贝节点对象的索引键值的回调操作接口（可以设置为 XRBT_NULL）。
     * @note  创建 x_rbtree_t 对象时，若该参数为 XRBT_NULL，则取内部默认的操作接口。
     * 
     * @param [out] xrbt_dkey : 目标的索引键缓存。
     * @param [in ] xrbt_skey : 源数据的索引键缓存。
     * @param [in ] xrbt_size : 索引键缓存大小。
     * @param [in ] xbt_move  : 是否采用右值 move 操作进行数据拷贝（用于兼容 C++11 后的对象右值引用操作）。
     */
    xrbt_void_t (* xfunc_vkey_copyfrom)(xrbt_vkey_t xrbt_dkey,
                                        xrbt_vkey_t xrbt_skey,
                                        xrbt_size_t xrbt_size,
                                        xrbt_bool_t xbt_move);

    /**
     * @brief 析构节点对象的索引键值的回调操作接口（可以设置为 XRBT_NULL）。
     * @note  创建 x_rbtree_t 对象时，若该参数为 XRBT_NULL，则取内部默认的操作接口。
     */
    xrbt_void_t (* xfunc_vkey_destruct)(xrbt_vkey_t xrbt_vkey,
                                        xrbt_size_t xrbt_size);

} xrbt_allocator_t;

//====================================================================

// 
// 红黑树的相关操作接口
// 

/**********************************************************/
/**
 * @brief 创建 x_rbtree_t 对象。
 * 
 * @param [in ] xst_ksize  : 索引键数据类型所需的缓存大小（如 sizeof 值）。
 * @param [in ] xfunc_kcmp : 比较节点索引键值的回调接口（若为 XRBT_NULL，则取内部默认接口）。
 * @param [in ] xallocator : 节点缓存分配器（参看 xrbt_allocator_t 定义）。
 * 
 * @return x_rbtree_ptr
 *         - 成功，返回 x_rbtree_t 对象；
 *         - 失败，返回 XRBT_NULL；
 */
x_rbtree_ptr xrbtree_create(xrbt_size_t xst_ksize,
                            xfunc_vkey_cmp_t xfunc_kcmp,
                            xrbt_allocator_t * xallocator);

/**********************************************************/
/**
 * @brief 销毁 x_rbtree_t 对象。
 */
xrbt_void_t xrbtree_destroy(x_rbtree_ptr xthis_ptr);

/**********************************************************/
/**
 * @brief 清除 x_rbtree_t 对象中的所有节点。
 */
xrbt_void_t xrbtree_clear(x_rbtree_ptr xthis_ptr);

/**********************************************************/
/**
 * @brief 返回 x_rbtree_t 对象的 节点缓存分配器。
 */
xrbt_allocator_t * xrbtree_allocator(x_rbtree_ptr xthis_ptr);

/**********************************************************/
/**
 * @brief 返回 x_rbtree_t 对象中的节点数量。
 */
xrbt_size_t xrbtree_size(x_rbtree_ptr xthis_ptr);

/**********************************************************/
/**
 * @brief 判断 x_rbtree_t 对象中节点数量是否为空。
 */
xrbt_bool_t xrbtree_empty(x_rbtree_ptr xthis_ptr);

/**********************************************************/
/**
 * @brief 返回 x_rbtree_t 对象中的左手臂长度。
 */
xrbt_size_t xrbtree_left_length(x_rbtree_ptr xthis_ptr);

/**********************************************************/
/**
 * @brief 返回 x_rbtree_t 对象中的右手臂长度。
 */
xrbt_size_t xrbtree_right_length(x_rbtree_ptr xthis_ptr);

/**********************************************************/
/**
 * @brief 向 x_rbtree_t 对象插入新节点。
 * @note  回调设置索引键值时，不使用 move 操作方式。
 * 
 * @param [in ] xthis_ptr : 红黑树对象。
 * @param [in ] xrbt_vkey : 新节点的索引键值。
 * @param [out] xbt_ok    : 返回操作成功的标识。
 * 
 * @return x_rbnode_iter
 *         - 返回对应节点。
 */
x_rbnode_iter xrbtree_insert(
    x_rbtree_ptr xthis_ptr, xrbt_vkey_t xrbt_vkey, xrbt_bool_t * xbt_ok);

/**********************************************************/
/**
 * @brief 向 x_rbtree_t 对象插入新节点。
 * @note  回调设置索引键值时，使用 move 操作方式。
 * 
 * @param [in ] xthis_ptr : 红黑树对象。
 * @param [in ] xrbt_mkey : 新节点的索引键值。
 * @param [out] xbt_ok    : 返回操作成功的标识。
 * 
 * @return x_rbnode_iter
 *         - 返回对应节点。
 */
x_rbnode_iter xrbtree_insert_mkey(
    x_rbtree_ptr xthis_ptr, xrbt_vkey_t xrbt_mkey, xrbt_bool_t * xbt_ok);

/**********************************************************/
/**
 * @brief 从 x_rbtree_t 对象中删除指定节点。
 */
xrbt_void_t xrbtree_erase(x_rbtree_ptr xthis_ptr, x_rbnode_iter xiter_node);

/**********************************************************/
/**
 * @brief 从 x_rbtree_t 对象中删除指定节点。
 */
xrbt_bool_t xrbtree_erase_vkey(x_rbtree_ptr xthis_ptr, xrbt_vkey_t xrbt_vkey);

/**********************************************************/
/**
 * @brief 将节点对象停靠（插入）到红黑树中。
 * @note
 * xiter_node 必须处于分离状态（参看 @see xrbtree_iter_is_undocked()），
 * 而且其索引键必须 与 xthis_ptr 存储的节点索引键类型相同。
 * 
 * @param [in ] xthis_ptr  : 红黑树对象。
 * @param [in ] xiter_node : 停靠操作的节点对象。
 * 
 * @return x_rbnode_iter
 *         - 返回的 节点对象 == xiter_node，停靠成功。
 *         - 返回的 节点对象 != xiter_node，停靠失败（索引键值冲突）。
 */
x_rbnode_iter xrbtree_dock(x_rbtree_ptr xthis_ptr, x_rbnode_iter xiter_node);

/**********************************************************/
/**
 * @brief 将节点对象从红黑树中分离（移除）出来。
 * @note xiter_node 必须 已经隶属于 xthis_ptr 。
 * 
 * @param [in ] xthis_ptr  : 红黑树对象。
 * @param [in ] xiter_node : 分离操作的节点对象。
 * 
 * @return x_rbnode_iter
 *         - 返回的 节点对象 == xiter_node。
 */
x_rbnode_iter xrbtree_undock(x_rbtree_ptr xthis_ptr, x_rbnode_iter xiter_node);

/**********************************************************/
/**
 * @brief 在 x_rbtree_t 对象中查找指定节点。
 * @note  若返回 NIL 则表示 x_rbtree_t 对象不包含该节点键值。
 */
x_rbnode_iter xrbtree_find(x_rbtree_ptr xthis_ptr, xrbt_vkey_t xrbt_vkey);

/**********************************************************/
/**
 * @brief 在 x_rbtree_t 对象中查找下边界节点。
 * @note  若返回 NIL 则表示 xrbt_vkey < 正向起始节点的索引键值。
 */
x_rbnode_iter xrbtree_lower_bound(x_rbtree_ptr xthis_ptr, xrbt_vkey_t xrbt_vkey);

/**********************************************************/
/**
 * @brief 在 x_rbtree_t 对象中查找上边界节点。
 * @note  若返回 NIL 则表示 xrbt_vkey > 反向起始节点的索引键值。
 */
x_rbnode_iter xrbtree_upper_bound(x_rbtree_ptr xthis_ptr, xrbt_vkey_t xrbt_vkey);

/**********************************************************/
/**
 * @brief 返回 x_rbtree_t 对象的根节点。
 */
x_rbnode_iter xrbtree_root(x_rbtree_ptr xthis_ptr);

/**********************************************************/
/**
 * @brief 返回 x_rbtree_t 对象的正向遍历操作的起始节点。
 */
x_rbnode_iter xrbtree_begin(x_rbtree_ptr xthis_ptr);

/**********************************************************/
/**
 * @brief 返回 x_rbtree_t 对象的正向遍历操作的终止节点。
 */
x_rbnode_iter xrbtree_end(x_rbtree_ptr xthis_ptr);

/**********************************************************/
/**
 * @brief 返回（正向）下一个节点对象。
 */
x_rbnode_iter xrbtree_next(x_rbnode_iter xiter_node);

/**********************************************************/
/**
 * @brief 返回 x_rbtree_t 对象的反向遍历操作的起始节点。
 */
x_rbnode_iter xrbtree_rbegin(x_rbtree_ptr xthis_ptr);

/**********************************************************/
/**
 * @brief 返回 x_rbtree_t 对象的反向遍历操作的终止节点。
 */
x_rbnode_iter xrbtree_rend(x_rbtree_ptr xthis_ptr);

/**********************************************************/
/**
 * @brief 返回（反向）下一个节点对象。
 */
x_rbnode_iter xrbtree_rnext(x_rbnode_iter xiter_node);

/**********************************************************/
/**
 * @brief 返回 节点对象 指向的索引键值。
 */
xrbt_vkey_t xrbtree_iter_vkey(x_rbnode_iter xiter_node);

/**********************************************************/
/**
 * @brief 判断 节点对象 是否为 NIL 。
 */
xrbt_bool_t xrbtree_iter_is_nil(x_rbnode_iter xiter_node);

/**********************************************************/
/**
 * @brief 判断 节点对象 是否为 非停靠状态（即其已被分离出红黑树）。
 */
xrbt_bool_t xrbtree_iter_is_undocked(x_rbnode_iter xiter_node);

/**********************************************************/
/**
 * @brief 返回 节点对象 所隶属的红黑树。
 * @note  内部会进行节点遍历操作，使用时应注重效率问题。
 */
x_rbtree_ptr xrbtree_iter_tree(x_rbnode_iter xiter_node);

////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}; // extern "C"
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// declare C API for the rbtree node type

#define XRBTREE_TYPE_API(_Kty)                                      \
																	\
xrbt_bool_t xrbtree_xfunc_##_Kty##_compare(                         \
    xrbt_vkey_t xrbt_lkey,                                          \
    xrbt_vkey_t xrbt_rkey,                                          \
    xrbt_size_t xrbt_size)                                          \
{                                                                   \
    return (*(_Kty *)xrbt_lkey < *(_Kty *)xrbt_rkey);               \
}                                                                   \
                                                                    \
xrbt_void_t xrbtree_xfunc_##_Kty##_copyfrom(                        \
    xrbt_vkey_t xrbt_dkey,                                          \
    xrbt_vkey_t xrbt_skey,                                          \
    xrbt_size_t xrbt_size,                                          \
    xrbt_bool_t xbt_move)                                           \
{                                                                   \
    *(_Kty *)xrbt_dkey = *(_Kty *)xrbt_skey;                        \
}                                                                   \
                                                                    \
inline xrbt_allocator_t xrbtree_default_allocator_##_Kty()          \
{                                                                   \
    xrbt_allocator_t xallocator =                                   \
    {                                                               \
        XRBT_NULL,                                                  \
        XRBT_NULL,                                                  \
        &xrbtree_xfunc_##_Kty##_copyfrom,                           \
        XRBT_NULL                                                   \
    };                                                              \
    return xallocator;                                              \
}                                                                   \
                                                                    \
x_rbtree_ptr xrbtree_create_##_Kty(                                 \
    xfunc_vkey_cmp_t xfunc_kcmp, xrbt_allocator_t * xallocator)     \
{                                                                   \
    if (XRBT_NULL == xfunc_kcmp)                                    \
    {                                                               \
        xfunc_kcmp = &xrbtree_xfunc_##_Kty##_compare;               \
    }                                                               \
                                                                    \
    if (XRBT_NULL == xallocator)                                    \
    {                                                               \
        static xrbt_allocator_t _S_allocator =                      \
            xrbtree_default_allocator_##_Kty();                     \
        xallocator = &_S_allocator;                                 \
    }                                                               \
                                                                    \
    return xrbtree_create(sizeof(_Kty), xfunc_kcmp, xallocator);    \
}                                                                   \
                                                                    \
inline x_rbnode_iter xrbtree_insert_##_Kty(                         \
    x_rbtree_ptr xthis_ptr, _Kty xkey, xrbt_bool_t * xbt_ok)        \
{                                                                   \
    return xrbtree_insert(xthis_ptr, (_Kty *)(&xkey), xbt_ok);      \
}                                                                   \
                                                                    \
inline x_rbnode_iter xrbtree_insert_m_##_Kty(                       \
    x_rbtree_ptr xthis_ptr, _Kty xkey, xrbt_bool_t * xbt_ok)        \
{                                                                   \
    return xrbtree_insert_mkey(xthis_ptr, (_Kty *)(&xkey), xbt_ok); \
}                                                                   \
                                                                    \
inline xrbt_bool_t xrbtree_erase_##_Kty(                            \
    x_rbtree_ptr xthis_ptr, _Kty xkey)                              \
{                                                                   \
    return xrbtree_erase_vkey(xthis_ptr, (_Kty *)(&xkey));          \
}                                                                   \
                                                                    \
inline x_rbnode_iter xrbtree_find_##_Kty(                           \
    x_rbtree_ptr xthis_ptr, _Kty xkey)                              \
{                                                                   \
    return xrbtree_find(xthis_ptr, (_Kty *)(&xkey));                \
}                                                                   \
                                                                    \
inline x_rbnode_iter xrbtree_lower_bound_##_Kty(                    \
    x_rbtree_ptr xthis_ptr, _Kty xkey)                              \
{                                                                   \
    return xrbtree_lower_bound(xthis_ptr, (_Kty *)(&xkey));         \
}                                                                   \
                                                                    \
inline x_rbnode_iter xrbtree_upper_bound_##_Kty(                    \
    x_rbtree_ptr xthis_ptr, _Kty xkey)                              \
{                                                                   \
    return xrbtree_upper_bound(xthis_ptr, (_Kty *)(&xkey));         \
}                                                                   \
                                                                    \
inline _Kty xrbtree_ikey_##_Kty(                                    \
    x_rbnode_iter xiter_node)                                       \
{                                                                   \
    return *(_Kty *)xrbtree_iter_vkey(xiter_node);                  \
}                                                                   \

////////////////////////////////////////////////////////////////////////////////
// declare C++ API for the rbtree node type

#ifdef __cplusplus

#if __cplusplus >= 201103L
#include <utility> // for std::move()
#endif // __cplusplus >= 201103L

template< class _Kty >
inline xrbt_bool_t xrbtree_xfunc_vkey_cmp(xrbt_vkey_t xrbt_lkey,
                                          xrbt_vkey_t xrbt_rkey,
                                          xrbt_size_t xrbt_size)
{
    return (*(_Kty *)xrbt_lkey < *(_Kty *)xrbt_rkey);
}

template< class _Kty >
inline xrbt_void_t xrbtree_xfunc_copyfrom(xrbt_vkey_t xrbt_dkey,
                                          xrbt_vkey_t xrbt_skey,
                                          xrbt_size_t xrbt_size,
                                          xrbt_bool_t xbt_move)
{
#if __cplusplus >= 201103L
    if (xbt_move)
        *(_Kty *)xrbt_dkey = std::move(*(_Kty *)xrbt_skey);
    else
#endif // __cplusplus >= 201103L
        *(_Kty *)xrbt_dkey = *(_Kty *)xrbt_skey;
}

template< class _Kty >
inline xrbt_void_t xrbtree_xfunc_destruct(xrbt_vkey_t xrbt_vkey,
                                          xrbt_size_t xrbt_size)
{
    (*(_Kty *)xrbt_vkey).~_Kty();
}

template< class _Kty >
inline xrbt_allocator_t xrbtree_default_allocator(void)
{
    xrbt_allocator_t xallocator =
    {
        XRBT_NULL,
        XRBT_NULL,
        &xrbtree_xfunc_copyfrom< _Kty >,
        &xrbtree_xfunc_destruct< _Kty >
    };

    return xallocator;
}

template< class _Kty >
inline x_rbtree_ptr xrbtree_create_k(xfunc_vkey_cmp_t xfunc_kcmp = XRBT_NULL,
                                     xrbt_allocator_t * xallocator = XRBT_NULL)
{
    if (XRBT_NULL == xfunc_kcmp)
    {
        xfunc_kcmp = &xrbtree_xfunc_vkey_cmp< _Kty >;
    }

    if (XRBT_NULL == xallocator)
    {
        static xrbt_allocator_t _S_allocator =
            xrbtree_default_allocator< _Kty >();
        xallocator = &_S_allocator;
    }

    return xrbtree_create(sizeof(_Kty), xfunc_kcmp, xallocator);
}

template< class _Kty >
inline x_rbnode_iter xrbtree_insert_k(x_rbtree_ptr xthis_ptr,
                                      const _Kty & xkey,
                                      xrbt_bool_t * xbt_ok = XRBT_NULL)
{
    return xrbtree_insert(xthis_ptr, const_cast< _Kty * >(&xkey), xbt_ok);
}

#if __cplusplus >= 201103L
template< class _Kty >
inline x_rbnode_iter xrbtree_insert_k(x_rbtree_ptr xthis_ptr,
                                      _Kty && xkey,
                                      xrbt_bool_t * xbt_ok = XRBT_NULL)
{
    return xrbtree_insert_mkey(xthis_ptr, const_cast< _Kty * >(&xkey), xbt_ok);
}
#endif // __cplusplus >= 201103L

template< class _Kty >
inline xrbt_bool_t xrbtree_erase_k(x_rbtree_ptr xthis_ptr, const _Kty & xkey)
{
    return xrbtree_erase_vkey(xthis_ptr, const_cast< _Kty * >(&xkey));
}

template< class _Kty >
inline x_rbnode_iter xrbtree_find_k(x_rbtree_ptr xthis_ptr, const _Kty & xkey)
{
    return xrbtree_find(xthis_ptr, const_cast< _Kty * >(&xkey));
}

template< class _Kty >
inline x_rbnode_iter xrbtree_lower_bound_k(x_rbtree_ptr xthis_ptr, const _Kty & xkey)
{
    return xrbtree_lower_bound(xthis_ptr, const_cast< _Kty * >(&xkey));
}

template< class _Kty >
inline x_rbnode_iter xrbtree_upper_bound_k(x_rbtree_ptr xthis_ptr, const _Kty & xkey)
{
    return xrbtree_upper_bound(xthis_ptr, const_cast< _Kty * >(&xkey));
}

template< class _Kty >
inline _Kty & xrbtree_ikey_k(x_rbnode_iter xiter_node)
{
    return *(static_cast< _Kty * >(xrbtree_iter_vkey(xiter_node)));
}

#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////

#endif // __XRBTREE_H__