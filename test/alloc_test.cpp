
#define MINISTL_DEBUG

#include <iostream>
#include <vector>
#include <string>
#include "../include/allocator.h"

using std::cout;
using std::endl;

struct alloc_test {
    alloc_test() : i(1), d(3.14), s("alloc_test") {}
    void show() {
        cout << "i: " << i << endl;
        cout << "d: " << d << endl;
        cout << "s: " << s << endl;
    }
    int i;
    double d;
    std::string s;
};

int main () {
    // std::vector<int, ministl::allocator<int>> intVec;
    // for (int i = 0; i < 10; ++i)
    //     intVec.push_back(i);
    // for (int i = 0; i < 10; ++i)
    //     cout << intVec[i] << endl;
    std::vector<alloc_test, ministl::allocator<alloc_test>> testVec;
    alloc_test t1;
    cout << "sizeof(alloc_test): " << sizeof(alloc_test) << endl;
    testVec.push_back(t1);
    testVec.push_back(alloc_test());
    cout << testVec.size() << endl;
    testVec[0].show();
    testVec[1].show();
    testVec.erase(testVec.begin());
    cout << testVec.size() << endl;
    testVec[0].show();
}
