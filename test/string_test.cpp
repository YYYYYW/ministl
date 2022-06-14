#define MINISTL_DEBUG

#include "../include/string.h"
#include <iostream>
#include <vector>

using namespace ministl;
using std::cout;
using std::endl;

int main() {
    string a = "123";
    string b;
    b = a;
    cout << "********" << endl;
    std::vector<string> v;
    v.push_back(a);
    cout << "--------" << endl;
    v.push_back(b);
    cout << "+++++++" << endl;
    v.push_back(b);
    cout << "=======" << endl;
    v.push_back(b);
    return 0;
}