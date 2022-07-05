
#ifndef MINISTL_ITERATOR_H
#define MINISTL_ITERATOR_H

#include <cstddef>

namespace ministl {

struct input_iterator_tag { };
struct output_iterator_tag { };
struct forward_iterator_tag : public input_iterator_tag { };
struct bidirectional_iterator_tag : public forward_iterator_tag { };
struct random_access_iterator_tag : public bidirectional_iterator_tag { };


template <typename I>
struct iterator_traits {
    typedef typename I::iterator_category   iterator_category;
    typedef typename I::value_type          value_type;
    typedef typename I::difference_type     difference_type;
    typedef typename I::pointer             pointer;
    typedef typename I::reference           refernece;
};


// 针对原生指针的偏特化
template <typename T>
struct iterator_traits<T*> {
    typedef random_access_iterator_tag  iterator_category;
    typedef T                           value_type;
    typedef ptrdiff_t                   difference_type;
    typedef T*                          pointer;
    typedef T&                          reference;

};

// 针对原生的pointer-to-const的偏特化
template <typename T>
struct iterator_traits<const T*> {
    typedef random_access_iterator_tag  iterator_category;
    typedef T                           value_type;
    typedef ptrdiff_t                   difference_type;
    typedef const T*                    pointer;
    typedef const T&                    reference;  
};

// 获取iterator_category的函数
template <typename Iterator>
inline typename iterator_traits<Iterator>::iterator_category
iterator_category(const Iterator&) {
    typedef typename iterator_traits<Iterator>::iterator_category category;
    return category();
}

// 获取value_type的函数
template <typename Iterator>
inline typename iterator_traits<Iterator>::value_type*
value_type(const Iterator&) {
    return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}

// 获取difference_type的函数
template <typename Iterator>
inline typename iterator_traits<Iterator>::difference_type*
difference_type(const Iterator&) {
    return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
}

// 根据两个迭代器获得距离的函数
template <typename InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
_distance(InputIterator first, InputIterator last, input_iterator_tag) {
    typename iterator_traits<InputIterator>::difference_type n = 0;
    while (first != last) {
        ++first;
        ++n;
    }
    return n;
}

template <typename RandomAccessIterator>
inline typename iterator_traits<RandomAccessIterator>::difference_type
_distance(RandomAccessIterator first, RandomAccessIterator last, 
          random_access_iterator_tag) {
    return last - first;
}

template <typename InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last) {
    typedef typename iterator_traits<InputIterator>::iterator_category
        category;
    return _distance(first, last, category());
}

// advance 函数，将迭代器前进 n 步
template <typename InputIterator, typename Distance>
inline void _advance(InputIterator& i, Distance n, input_iterator_tag) {
    while (n--) ++i;
}

template <typename BidirectionalIterator, typename Distance>
inline void _advance(BidirectionalIterator& i, Distance n, 
                     bidirectional_iterator_tag) {
    if (n >= 0)
        while (n--) ++i;
    else
        while (n++) --i;
}

template <typename RandomAccessIterator, typename Distance>
inline void _advance(RandomAccessIterator& i, Distance n) {
    i += n;
}

template <typename InputIterator, typename Distance>
inline void advance(InputIterator& i, Distance n) {
    _advance(i, n, iterator_category(i));
}



}

#endif // MINISTL_ITERATOR_H