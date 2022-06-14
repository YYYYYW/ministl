#ifndef MINISTL_STRING_H
#define MINISTL_STRING_H

#include <iostream>

#ifdef MINISTL_ALLOCATOR_H
    #include <allocator.h>
#else
    #include <memory>
#endif

namespace ministl {

/**
 * 需要一个C类型的字符串作为存储，一个指针指向头，一个指针指向尾。
 * 目前使用std中的配置器
 * TODO allocator
 */
class string {

public:
    // 默认构造函数
    string() : elements(nullptr), cap(nullptr) {
        #ifdef MINISTL_DEBUG
            std::cout << "default constructor(string)" << std::endl;
        #endif
    }

    // C字符串构造函数
    string(const char *);

    // 复制构造函数
    string(const string &);

    /**
     * 移动构造函数
     * noexcept表示告诉标准库该构造函数不抛出任何异常
     * 这是因为移动不会分配任何新内存，仅仅是接管了实参的内存空间
     */
    string(string &&) noexcept;

    // 赋值运算符
    string& operator= (const string &);
    string& operator= (string &&) noexcept;

    // 析构函数
    ~string();

    char * begin() const { return elements; }
    char * end() const { return cap; }

private: 
    static std::allocator<char> alloc;
    std::pair<char *, char *> alloc_n_copy(const char *, const char *);
    void free();
    char *elements;
    char *cap;
};

std::allocator<char> string::alloc;

string::string(const char * s) {
    auto s1 = s;
    while (*s1) {
        ++s1;
    }
    auto newstr = alloc_n_copy(s, s1);
    elements = newstr.first;
    cap = newstr.second;
    #ifdef MINISTL_DEBUG
        std::cout << "string constructor with c_string" << std::endl;
    #endif
}

string::string(const string & s) {
    auto newdata = alloc_n_copy(s.begin(), s.end());
    elements = newdata.first;
    cap = newdata.second;

    #ifdef MINISTL_DEBUG
        std::cout << "string copy constructor" << std::endl;
    #endif
}

// 移动相当于偷取了参数s中的资源，然乎把s中的置为nullptr，并没有内存分配
string::string(string && s) noexcept : elements(s.elements), cap(s.cap) {
    s.elements = s.cap = nullptr;
    #ifdef MINISTL_DEBUG
        std::cout << "string move constructor" << std::endl;
    #endif
}

// 对于二元运算符，统一使用rhs(right hand side)表示运算符右侧，lhs(left hand side)表示运算符左侧
string& string::operator= (const string & rhs) {
    auto newdata = alloc_n_copy(rhs.begin(), rhs.end());
    free();
    elements = newdata.first;
    cap = newdata.second;
    #ifdef MINISTL_DEBUG
        std::cout << "string assignment operator" << std::endl;
    #endif
    return *this;
}

string& string::operator= (string && rhs) noexcept {
    // 对于移动赋值，都需要先判断两边是否是相等
    if (this != &rhs) {
        free();
        elements = rhs.elements;
        cap = rhs.cap;
        rhs.elements = nullptr;
        rhs.cap = nullptr;
    }
    #ifdef MINISTL_DEBUG
        std::cout << "string move assignment operator" << std::endl;
    #endif
    return *this;
}

string::~string() {
    free();
    #ifdef MINISTL_DEBUG
        std::cout << "string destructor" << std::endl;
    #endif
}

void string::free() {
    if (elements) {
        for (auto p = cap; p != elements; ) {
            alloc.destroy(--p);
        }
        alloc.deallocate(elements, cap-elements);
    }
}

std::pair<char *, char *> string::alloc_n_copy (const char * beg, const char * end) {
    auto newstr = alloc.allocate(end - beg);
    return {newstr, std::uninitialized_copy(beg, end, newstr)};
}

}


#endif //MINISTL_STRING_H