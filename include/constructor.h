
#ifndef MINISTL_CONSTRUCTOR_H
#define MINISTL_CONSTRUCTOR_H

namespace ministl {

#include <new> // for placement new

// 用于构造对象
template <typename T1, typename T2>
inline void construct(T1* p, const T2& value) {
    /**
     * 形如 new(p) A(value)
     * 这个 new 在 #include <new.h> 中，叫做 placement new
     * 不会分配内存，会在已分配的内存地址 p 上执行 A 的构造函数，value为构造函数的参数 
     */
    new(p) T1(value);       // placement new
}

template <typename T1, typename T2>
inline void construct(T1* p) {
    new(p) T1();
}

// 用于析构对象
template <typename T>
inline void destroy(T* ptr) {
    ptr->~T();
}

// 用于析构对象，参数为迭代器
template <typename ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last) {
    _destroy(first, last, value_type(first));
}

// 判断元素的型别是否有 trivial destructor
template <typename ForwardIterator, typename T>
inline void _destroy(ForwardIterator first, ForwardIterator last, T*) {
    typedef typename type_traits<T>::has_trivial_destructor trivial_destructor;
    _destroy_aux(first, last, trivial_destructor());
}

// 如果元素的型别有 non-trivial destructor 
template <typename ForwardIterator>
inline void _destroy_aux(ForwardIterator first, ForwardIterator last, false_type) {
    for ( ; first < last; ++first)
        destroy(&*first);
}

// 如果元素的型别有 trivial destructor, 什么也不做
template <typename ForwardIterator>
inline void _destroy_aux(ForwardIterator first, ForwardIterator last, true_type) { }

// 基础类型的特化版本
inline void _destroy(char*, char*) {}
inline void _destroy(int*, int*) {}
inline void _destroy(long*, long*) {}
inline void _destroy(float*, float*) {}
inline void _destroy(double*, double*) {}
inline void _destroy(wchar_t*, wchar_t*) {}


}

#endif // MINISTL_CONSTRUCTOR_H