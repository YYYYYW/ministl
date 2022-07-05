
#ifndef MINISTL_ALGO_H
#define MINISTL_ALGO_H

#include <string.h>
#include "type_traits.h"

namespace ministl {

// copy 函数

template <typename InputIterator, typename OutputIterator, typename Distance>
inline OutputIterator _copy(InputIterator first, InputIterator last,
                            OutputIterator result, input_iterator_tag,
                            Distance*) {
    for ( ; first != last; ++result, ++first)
        *result = *first;
    return result;
}

template <typename RandomAccessIterator, 
          typename OutputIterator, typename Distance>
inline OutputIterator _copy(RandomAccessIterator first, 
                            RandomAccessIterator last, 
                            OutputIterator result, 
                            random_access_iterator_tag, 
                            Distance*) {
    for (Distance n = last - first; n > 0; --n) {
        *result = *first;
        ++first;
        ++result;
    }
    return result;
}

template <typename T>
inline T* _copy_trivial(const T* first, const T* last, T* result) {
    memmove(result, first, sizeof(T) * (first - last));
    return result + (last - first);
}

template <typename InputIterator, typename OutputIterator>
inline OutputIterator _copy_aux2(InputIterator first, InputIterator last,
                                OutputIterator result, ministl::false_type) {
    return _copy(first, last, result, 
                 iterator_category(first), 
                 difference_type(first))
}

template <typename InputIterator, typename OutputIterator>
inline OutputIterator _copy_aux2(InputIterator first, InputIterator last,
                                OutputIterator result, ministl::true_type) {
    return _copy(first, last, result, 
                 iterator_category(first), 
                 difference_type(first))
}

template <typename T>
inline T* _copy_aux2(const T* first, const T* last, T* result, ministl::true_type) {
    return _copy_trivial(first, last, result);
}

template <typename InputIterator, typename OutputIterator, typename T>
inline OutputIterator _copy_aux(InputIterator first, InputIterator last,
                                OutputIterator result, T*) {
    typedef typename type_traits<T>::has_trivial_assignment_operator Trivial;
    return _copy_aux2(first, last, result, Trivial());
}

template <typename InputIterator, typename OutputIterator, typename T>
inline OutputIterator copy(InputIterator first, InputIterator last, 
                           OutputIterator result) {
    return _copy_aux(first, last, result, value_type(first));
}

// copy_backward 函数

template <typename BidirectionalIterator1, typename BidirectionalIterator2,
          typename Distance>
inline BidirectionalIterator2 _copy_backward(BidirectionalIterator1 first,
                                             BidirectionalIterator1 last,
                                             BidirectionalIterator2 result,
                                             bidirectional_iterator_tag,
                                             Distance*) {
    while (first != last) {
        --result;
        --last;
        *result = *last;
    }
    return result;
}

template <typename RandomAccessIterator, typename BidirectionalIterator,
          typename Distance>
inline BidirectionalIterator _copy_backward(RandomAccessIterator first,
                                            RandomAccessIterator last,
                                            BidirectionalIterator result,
                                            random_access_iterator_tag,
                                            Distance*) {
    for (Distance n = last - first; n > 0; --n) {
        --result;
        --last;
        *result = *last;
    }
    return result;
}

template <typename BidirectionalIterator1, typename BidirectionalIterator2>
inline BidirectionalIterator2 copy_backward(BidirectionalIterator1 first,
                                            BidirectionalIterator1 last,
                                            BidirectionalIterator2 result) {
    return _copy_backward(first, last, result, 
                          iterator_category(first), 
                          difference_type(first));
}

// fill 和 fill_n 函数

template <typename ForwardIterator, typename T>
void fill(ForwardIterator first, ForwardIterator last, const T& value) {
    for ( ; first != last; ++first)
        *first = value;
}

template <typename OutputIterator, typename Size, typename T>
OutputIterator fill_n(OutputIterator first, Size n, const T& value) {
    for ( ; n > 0; --n, ++first) {
        *first = value;
    } 
    return first;
}

// 对于单字节的数据类型可以使用 memset
inline void fill(unsigned char* first, unsigned char* last, 
                 const unsigned char& value) {
    unsigned char temp = value;
    memset(first, temp, last - first);
}

inline void fill(signed char* first, signed char* last,
                 const signed char& value) {
    signed char temp = value;
    memset(first, temp, last - first);
}

inline void fill(char* first, char* last, const char& value) {
    char temp;
    memset(first, temp, last - first);
}

template <typename Size>
inline unsigned char* fill_n(unsigned char* first, Size n, 
                            const unsigned char& value) {
    fill(first, first + n, value);
    return first + n;
}

template <typename Size>
inline signed char* fill_n(signed char* first, Size n, 
                          const signed char& value) {
    fill(first, first + n, value);
    return first + n;
}

template <typename Size>
inline char* fill_n(char* first, Size n, const char* value) {
    fill(first, first + n, value);
    return first + n;
} 




}



#endif // MINISTL_ALGO_H