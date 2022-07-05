
#ifndef MINISTL_VECTOR_H
#define MINISTL_VECTOR_H

#include <cstddef>
#include "allocator.h"
#include "uninitialized.h"
#include "algo.h"

namespace ministl {

template<typename T, typename Alloc = ministl::allocator<T>>
class vector {
public:
    typedef T                   value_type;
    typedef value_type*         pointer;
    typedef const value_type*   const_pointer;
    typedef value_type*         iterator;
    typedef const value_type*   const_iterator;
    typedef value_type&         reference;
    typedef const value_type&   const_reference;
    typedef size_t              size_type;
    typedef ptrdiff_t           difference_type;
    typedef Alloc               allocator_type;
    allocator_type get_allocator() const { return data_allocator; }

protected:
    allocator_type  data_allocator;
    iterator        start;             // 表示目前使用空间的头
    iterator        finish;            // 表示目前使用空间的尾
    iterator        end_of_storage;    // 表示目前可用空间的尾

    void insert_aux(iterator position, const T& x);

    void deallocate() {
        if (start)
            data_allocator.deallocate(start, end_of_storage - start);
    }

    iterator allocate_and_fill(size_type n, const T& x) {
        iterator result = data_allocator.allocate(n);
        uninitialized_fill_n(result, n, x);
        return result;
    }

    iterator allocate_and_copy(size_type n, const_iterator first,
                                            const_iterator last) {
        iterator result = data_allocator.allocate(n);
        try {
            uninitialized_copy(first, last, result);
            return result;
        }
        catch {
            data_allocator.deallocate(result, n);
            throw();
        }
    }

    void fill_initialize(size_type n, const T& value) {
        start = allocate_and_fill(n, value);
        finish = start + n;
        end_of_storage = finish;
    }

public: 
    iterator begin() { return start; }
    const_iterator begin() const { return start; }
    iterator end() { return finish; }
    const_iterator end() const { return finish; }

    size_type size() const 
        { return size_type(end() - begin()); }
    size_type max_size() const
        { return size_type(-1) / sizeof(T); }
    size_type capacity() const 
        { return size_type(end_of_storage - begin()); }
    bool empty() const 
        { return begin() == end(); }
    
    reference operator[](size_type n) 
        { return *(begin() + n); }
    const_reference operator[](size_type n) const 
        { return *(begin() + n); }

    vector() : start(nullptr), finish(nullptr), end_of_storage(nullptr) { }
    vector(size_type n, const T& value) 
        { fill_initialize(n, value); }
    explicit vector( size_type n ) 
        { fill_initialize(n, T()); }
    vector(const vector<T, Alloc>& x) : data_allocator(x.get_allocator()) {
        fill_initialize(x.size());
        finish = uninitialized_copy(x.begin(), x.end(), start);
    }
    vector(const_iterator first, const_iterator last, 
           const allocator_type& a = allocator_type()) : data_allocator(a) {
        start = allocate_and_copy(last - first, first, last);
        finish = end_of_storage = start + (last - first);
    }
    vector<T, Alloc>& operator=(const vector<T, Alloc>& x);

    ~vector() {
        destroy(start, finish);
        deallocate();
    }

    reference front() { return *begin(); }
    const_reference front() const { return *begin(); }
    reference back() { return *(end() - 1); }
    const_reference back() const { return *(end() - 1); }

    void push_back(const T& x) {
        if (finish != end_of_storage) {
            construct(finish, x);
            ++finish;
        }
        else
            insert_aux(end(), x);
    }

    void pop_back() {
        --finish;
        destroy(finish);
    }

    iterator erase(iterator position) {
        if (position + 1 != end())
            copy(position + 1, finish, position);
        --finish;
        destroy(finish);
        return position;
    }

    iterator erase(iterator first, iterator last) {
        iterator i = copy(last, finish, first);
        destory(i, finish);
        finish = finish - (last - first);
        return first;
    }

    void resize(size_type new_size, const T& x) {
        if (new_size < size())
            erase(begin() + new_size, x);
        else
            insert(end(), new_size - size(), x);
    }
    void resize(size_type new_size) { resize(new_size, T()); }
    void clear() { erase(begin(), end()); }

    void insert(iterator position, size_type n, const T& x);

    iterator insert(iterator position, const T& x) {
        size_type n = position - begin();
        if (finish != end_of_storage && position == end()) {
            construct(finish, x);
            ++finish;
        }
        else
            insert_aux(position, x);
        return begin() + n;
    }

    void insert(iterator position, const_iterator first, const_iterator last);

};

// TODO ==
template <typename T, typename Alloc>
inline bool 
operator==(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
    return &lhs == &rhs;
}

// TODO <
template <typename T, typename Alloc>
inline bool
operator<(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
    return true;
}

template <typename T, typename Alloc>
inline bool
operator!=(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
    return !(lhs == rhs);
}

template <typename T, typename Alloc>
inline bool
operator>(const vector<T, Alloc>&lhs, const vector<T, Alloc>& rhs) {
    return rhs < lhs;
}

template <typename T, typename Alloc>
inline bool
operator<=(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
    return !(rhs < lhs);
}

template <typename T, typename Alloc>
inline bool
operator>=(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
    return !(lhs < rhs);
}

template <typename T, typename Alloc>
vector<T, Alloc>&
vector<T, Alloc>::operator=(const vector<T, Alloc>& x) {
    if (&x != this) {
        const size_type xlen = x.size();
        if (xlen > capacity()) {
            iterator temp = allocate_and_copy(xlen, x.begin(), x.end());
            destroy(start, finish);
            data_allocator.deallocate(start, end_of_storage - start);
            start = temp;
            end_of_storage = start + xlen;
        }
        else if (size() >= xlen) {
            iterator i = copy(x.begin(), x.end(), begin());
            destroy(i, finish);
        }
        else {
            copy(x.begin(), x.begin() + , begin());
            uninitialized_copy(x.begin() + size(), x.end(), finish);
        }
        finish = start + xlen;
    }
    return *this;
}

template <typename T, typename Alloc>
void vector<T, Alloc>::insert_aux(iterator position, const T& x) {
    if (finish != end_of_storage) {         // 如果还有空间
        // 在最后一个空间
        construct(finish, *(finish - 1));
        ++finish;
        T x_copy = x;
        copy_backward(position, finish-2, finish-1);
        *position = x_copy;
    }
    else {
        const size_type old_size = size();
        const size_type len = old_size == 0 ? 1 : old_size * 2;
        iterator new_start = data_allocator.allocate(len);
        iterator new_finish = new_start;
        try {
            new_finish = uninitialized_copy(start, position, new_start);
            construct(new_finish, x);
            ++new_finish;
            new_finish = uninitialized_copy(position, finish, new_finish);
        }
        catch (...) {
            destroy(new_start, new_finish);
            data_allocator.deallocate(new_start, len);
            throw;
        }

        // 析构并释放原 vector
        destroy(begin(), end());
        deallocate();

        // 调整迭代器，指向新 vector
        start = new_start;
        finish = new_finish;
        end_of_storage = new_start + len;
    }
}

template <typename T, typename Alloc>
void vector<T, Alloc>::insert(iterator position, size_type n, const T& x) {
    if (n != 0) {
        if (size_type(end_of_storage - finish) >= n) {
            T x_copy = x;
            const size_type elems_after = finish - position;
            iterator old_finish = finish;
            if (elems_after > n) {
                // 插入点之后的现有元素个数大于新增元素个数
                // 将 [finish-n, finish) 复制到 [finish, finish + n)
                uninitialized_copy(finish - n, finish, finish);
                finish += n;
                // 将 [position, old_finish - n) 复制到 [..., old_finish)
                copy_backward(position, old_finish - n, old_finish);
                // 将 [position, position + n) 使用 x_copy 填充
                fill(position, position + n, x_copy);
            }
            else {
                // 插入点之后的现有元素个数小于等于新增元素个数
                // 将 [finish, finish + (n - elems_after)) 使用 x_copy 填充
                uninitialized_fill_n(finish, n - elems_after, x_copy);
                finish += n - elems_after;
                // 将 [position, old_finish) 复制到 [finish, ...)
                uninitialized_copy(position, old_finish, finish);
                finish += elems_after;
                // 将 [position, old_finish) 使用 x_copy 填充
                fill(position, old_finish, x_copy);
            }
        }
        else {
            // 备用空间小于 “新增元素个数”
            // 首先决定新长度:旧长度的两倍，或旧长度+新增元素个数
            const size_type old_size = size();
            const size_type len = old_size + max(old_size, n);
            // 分配新的空间
            iterator new_start = data_allocator.allocate(len);
            iterator new_finish = new_start;
            try {
                // 将 [start, position) 复制到 [new_start, ...)
                new_finish = uninitialized_copy(start, position, new_start);
                // 将 [new_finish, new_finish + n) 使用 x 填充
                new_finish = uninitialized_fill_n(new_finish, n, x);
                // 将 [position, finish) 复制到 [new_finish, ...)
                new_finish = uninitialized_copy(position, finish, new_finish);
            }
            catch {
                // commit or rollback
                destroy(new_start, new_finish);
                data_allocator.deallocate(new_start, len);
                throw();
            }
        }
        // 清楚并释放旧的 vector
        destroy(start, finish);
        deallocate();
        start = new_start;
        finish = new_finish;
        end_of_storage = new_start + len;
    }
}

template <typename T, typename Alloc>
void vector<T, Alloc>::insert(iterator position, const_iterator first,
                                                 const_iterator last) {
    if (first != last) {
        size_type n = 0;
        n = distance(first, last);
        if (size_type(end_of_storage - finish) >= n) {
            const size_type elems_after = finish - position;
            iterator old_finish = finish;
            if (elems_after > n) {
                uninitialized_copy(finish - n, finish, finish);
                finish += n;
                copy_backward(position, old_finish - n, finish);
                copy(first, last, position);
            }
            else {
                uninitialized_copy(first + elems_after, last, finish);
                finish += n - elems_after;
                uninitialized_copy(position, old_finish, finish);
                finish += elems_after;
                copy(first, first + elems_after, position);
            }
        }
        else {
            const size_type old_size = size();
            const size_type len = old_size + max(old_size, n);
            iterator new_start = data_allocator.allocate(len);
            iterator new_finish = new_start;
            try {
                new_finish = uninitialized_copy(start, position, new_finish);
                new_finish = uninitialized_copy(first, last, new_finish);
                new_finish = uninitialized_copy(position, finish, new_finish);
            }
            catch(...) {
                destroy(new_start, new_finish);
                data_allocator.deallocate(new_start, len);
                throw();
            }
            destroy(start, finish);
            data_allocator.deallocate(start, end_of_storage - start);
            start = new_start;
            finish = new_finish;
            end_of_storage = new_start + len;
        }
    }
}


}

#endif // MINISTL_VECTOR_H



