
#ifndef MINISTL_UNINITIALIZED_H
#define MINISTL_UNINITIALIZED_H

#include "iterator.h"
#include "allocator.h"
#include "type_traits.h"
#include "algo.h"

namespace ministl {

/**
 * uninitialized_fill_n 函数
 * 在迭代器 first 和 first+n 之间调用 copy constructor 初始化
 * 这里的内存是已经分配好了的，只需要初始化即可
 * 对于有 trivial constructor 的类型，调用 fill_n 函数，即每个元素直接使用 = 赋值
 * 对于 non-trivail constructor 的类型，调用 construct(p, value) 
 */
template <typename ForwardIterator, typename Size, typename T>
inline ForwardIterator uninitialized_fill_n(ForwardIterator first,
                                            Size n, const T& x) {
    // value_type 萃取出 first 的类型，为 T*
    return _uninitialized_fill_n(first, n, x, value_type(first));
}

template <typename ForwardIterator, typename Size, typename T, typename T1>
inline ForwardIterator _uninitialized_fill_n(ForwardIterator first,
                                             Size n, const T& x, T1*) {
    // POD, Plain Old Data, 标量型别或传统的 C struct 型别
    // POD 型别必然拥有 trivial ctor/dtor/copy/assignment 函数
    typedef typename type_traits<T1>::is_POD_type is_POD;
    return _uninitialized_fill_n_aux(first, n, x, is_POD());
}

// 如果是 POD 型别，则直接转为调用 fill_n() 函数
template <typename ForwardIterator, typename Size, typename T>
inline ForwardIterator
_uninitialized_fill_n_aux(ForwardIterator first, 
                          Size n, const T* x, ministl::true_type) {
    return fill_n(first, n, x);
}

// 如果不是 POD 型别，需要调用 constructor 来一个一个构造
// 要么产生所有元素，要么一个都不产生，即 commit or rollback
template <typename ForwardIterator, typename Size, typename T>
inline ForwardIterator
_uninitialized_fill_n_aux(ForwardIterator first, 
                          Size n, const T* x, ministl::false_type) {
    ForwardIterator cur = first;
    try {
        for ( ; n > 0; --n, ++cur) {
            // 因为 cur 是迭代器，所以 *cur 得到 T, 再 &T 得到 T*
            construct(&*cur, x);
        }
        return cur;
    } catch (...) {
        destroy(first, cur);
        throw;
    }
}

/**
 * uninitialized_copy 函数
 * 在迭代器 [result, result+(last-first)) 之间使用 [first, last) 中的元素初始化
 * 即调用复制构造函数复制 [first, last) 中的元素到 [result, result+(last-first)) 中
 * result 所指向的内存空间已经分配过了 
 */
template <typename InputIterator, typename ForwardIterator>
inline ForwardIterator 
uninitialized_copy(InputIterator first, InputIterator last, 
                   ForwardIterator result) {
    // 这里萃取的 result 的类型，因为要在 result 中构造相对应的对象
    return _uninitialized_copy(firs, last, result, value_type(result));
}

// 根据萃取得到的类型，判断是否该类型是否是 POD
template <typename InputIterator, typename ForwardIterator, typename T>
inline ForwardIterator
_uninitialized_copy(InputIterator first, InputIterator last, 
                    ForwardIterator result, T*) {
    typedef typename type_traits<T>::is_POD_type is_POD;
    return _uninitialized_copy_aux(first, last, result, is_POD());
}

// 是 POD, 调用算法文件中的 copy 函数
template <typename InputIterator, typename ForwardIterator>
inline ForwardIterator
_uninitialized_copy_aux(InputIterator first, InputIterator last, 
                        ForwardIterator result, ministl::true_type) {
    return copy(first, last, result);
}

// 不是 POD, 使用construct 构造对象， commit or rollback
template <typename InputIterator, typename ForwardIterator>
ForwardIterator
_uninitialized_copy_aux(InputIterator first, InputIterator last, 
                        ForwardIterator result, ministl::false_type) {
    ForwardIterator cur = result;
    try {
        for (; first != last; ++first, ++cur) {
            construct(&*cur, *first);
        }
        return cur;
    } catch (...) {
        destroy(result, cur);
        throw;
    }
}

// 针对 const char* 的特化版本，因为 memmove 的效率很高
inline char* uninitialized_copy(const char *first, const char *last, 
                                char *result) {
    memmove(result, first, last - first);
    return result + (last - first);
}

// 针对 const wchar_t* 的特化版本
inline wchar_t* uninitialized_copy(const wchar_t *first, const wchar_t *last, 
                                   wchar_t *result) {
    memmove(result, first, last - first);
    return result + (last - first);
}

/**
 * uninitialized_fill 函数
 * 对于 [first, last) 之间的内存空间使用 x 进行 copy constructor
 * 总体逻辑和以上两个函数类似
 */
template <typename ForwardIterator, typename T>
ForwardIterator
uninitialized_fill(ForwardIterator first, ForwardIterator last, 
                   const T& x) {
    _uninitialized_fill(first, last, x, value_type(first));
} 

template <typename ForwardIterator, typename T, typename T1>
ForwardIterator
_uninitialized_fill(ForwardIterator first, ForwardIterator last, 
                    const T& x, T1*) {
    typedef typename type_traits<T1>::is_POD_type is_POD;
    _uninitialized_fill_aux(first, last, x, is_POD());
}

template <typename ForwardIterator, typename T>
ForwardIterator
_uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, 
                        const T& x, ministl::true_type) {
    fill(first, last, x);
}

template <typename ForwardIterator, typename T>
ForwardIterator
_uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, 
                        const T& x, ministl::false_type) {
    ForwardIterator cur = first;
    try {
        for (; cur != last; ++cur) {
            construct(&*cur, x);
        }
        return cur;
    } catch {
        destroy(first, cur);
        throw;
    }
}


}


#endif // MINISTL_UNINITIALIZED_H