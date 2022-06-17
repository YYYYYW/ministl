/**
 * allocator的必要接口
 * allocator::value_type
 * allocator::pointer
 * allocator::const_pointer
 * allocator::reference
 * allocator::const_reference
 * allocator::size_type
 * allocator::difference_type
 * allocator::rebind
 *      一个嵌套的class template
 * allocator::allocator()
 *      default constructor
 * allocator::allocator(const allocator&)
 *      copy constructor
 * template <class U> allocator::allocator(const allocator<U>&)
 *      泛化的copy constructor
 * allocator::~allocator()
 *      destructor
 * pointer allocator::address(reference x) const
 *      返回某个对象的地址 a.address(x) == &x
 * const_pointer allocator::address(const_reference x) const
 *      返回某个const对象的地址 a.address(x) == &x
 * pointer allocator::allocate(size_type n, const void* = 0)
 *      配置空间，存储n个T对象，第二个参数是提示
 * void allocator::deallocate(pointer p, size_type n)
 *      归还之前配置的空间
 * size_type allocator::max_size() const
 *      返回可成功配置的最大量
 * void allocator::construct(pointer p, const T& x)
 *      等同于 new(const void* p) T(x)
 * void allocator::destroy(pointer p)
 *      等同于 p->~T()
 */


#ifndef MINISTL_ALLOCATOR_H
#define MINISTL_ALLOCATOR_H

#include <new>          // for placement new
#include <cstddef>      // for ptrdiff_t, size_t
#include <cstdlib>      // for exit()
#include <climits>      // for UINT_MAX
#include <iostream>     // for cerr
#include "alloc.h"

namespace ministl {

// 用于分配内存
template <class T>
inline T* _allocate(ptrdiff_t size, T*) {
    /**
     * new_handler set_new_handler(new_handler) throw();
     * 当 operator new 无法分配内存后，将会调用设置的参数 handler
     * 当传递 0 的时候，则是直接抛出 bad_alloc 异常
     * operator new 只会分配内存，不会调用构造函数
     */
    set_new_handler(0);
    T* tmp = (T*)(::operator new((size_t)(size * sizeof(T))));
    if (tmp == 0) {
        cerr << "out of memory" << endl;
        exit(1);
    }
    return tmp;
}

// 用于归还内存
template <typename T>
inline void _deallocate(T* buffer) {
    ::operator delete(buffer);
}

// 这里 allocator 中 allocate 和 deallocate 都是使用 operator new 实现



// 这里 allocator 中 allocate 和 deallocate 使用了 alloc.h 文件中内存池的实现
template<typename T, typename Alloc>
class allocator {

public:
    typedef T           value_type;
    typedef T*          pointer;
    typedef const T*    const_pointer;
    typedef T&          reference;
    typedef const T&    const_reference;
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;

    // rebind allocator of type U
    // TODO 还不知道这个东西有啥用
    template <class U>
    struct rebind {
        typedef allocator<U> other;
    }

    // hint used for locality
    static pointer allocate(size_type n, const void* hint = 0) {
        // return _allocate((difference_type) n, (pointer) 0 );
        return 0 == n ? nullptr : (pointer) Alloc::allocate(n * sizeof(value_type));
    }

    static pointer allocate(void) {
        return (pointer) Alloc::allocate(sizeof(value_type));
    }

    void deallocate(pointer p, size_type n) { 
        // _deallocate(p); 
        if (0 != n) Alloc::deallocate(p, n * sizeof(value_type));
    }

    void deallocate(pointer p) {
        Alloc::deallocate(p, sizeof(value_type));
    }


    void construct(pointer p, const T& value) {
        _construct(p, value);
    }
    
    void destroy(pointer p) {
        _destroy(p);
    }

    pointer address(reference x) {
        return (pointer) &x;
    }

    const_pointer const_address(const_reference x) {
        return (const_pointer) &x;
    }

    size_type max_size() const {
        return size_type(UINT_MAX/sizeof(T));
    }

};




}





#endif // MINISTL_ALLOCATOR_H