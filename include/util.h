
#ifndef MINISTL_UTIL_H
#define MINISTL_UTIL_H

#include <cstddef>
#include <type_traits> // for std::remove_reference

namespace ministl {

#ifdef MINISTL_DEBUG
#define _MINISTL_DEBUG(...) printf("(%s %s %d): ", __FILE__, __FUNCTION__, __LINE__); printf(__VA_ARGS__)
#else
#define _MINISTL_DEBUG(...)
#endif

/**
 * move 函数
 * remove_reference 获取元素类型，例如：
 * remove_reference<int&>::type 为 int
 * remove_reference<string&>::type 为 string
 * 模板参数的引用折叠：
 * X&   &    =>   X&
 * X&&  &&   =>   X&&
 * X&   &&   =>   X&
 * X&&  &    =>   X&
 * 从一个左值 static_cast 到一个右值引用时允许的
 */
template <typename T>
typename std::remove_reference<T>::type move(T&& t) noexcept {
    return static_cast<typename std::remove_reference<T>::type&&> (t);
}





}

#endif // MINISTL_UTIL_H