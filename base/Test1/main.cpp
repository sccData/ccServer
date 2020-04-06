#include "CircularBuffer.h"
#include <assert.h>
#include <string>
using namespace std;

template<class T, size_t N>
using tsCB = circular_buffer<T, N>;

template<class T, size_t N>
bool circular_buffer_equal(tsCB<T, N>& cb1, tsCB<T, N> cb2){
    auto it1 = cb1.begin(), it2 = cb2.begin();
    for (; it1 != cb1.end() && it2 != cb1.end(); ++it1, ++it2){
        if (*it1 != *it2)
            return false;
    }
    return (it1 == cb1.end() && it2 == cb2.end() && (*(cb1.end()) == *(cb2.end())));
}

void testCase1() {
    tsCB<int, 2> cb(1);
    assert(cb.size() == 1);
    assert(!cb.full());
    cb.push_back(1), cb.push_back(2);
    assert(cb.full());
    assert(!cb.empty());
    assert(cb.size() == 2);
}

void testCase2() {
    tsCB<string, 3> cb(3);
    cb[0] = "one", cb[1] = "two", cb[2] = "three";
    assert(*(cb.begin()) == "one" && *(cb.end()) == "three");
    assert(cb.front() == "one" && cb.back() == "three");
}

void testCase3(){
    tsCB<string, 3> cb(0);
    assert(cb.front() == std::string());

    cb.push_back("zxh"); cb.push_back("jwl");
    assert(cb.back() == "jwl");
    assert(cb.front() == "zxh");
}

void testCase4() {
	tsCB<string, 4> cb(0);
	cb.push_back("a");
	cb.push_back("b");
	cb.push_back("c");
	assert(circular_buffer_equal(cb, cb));
}


void testAllCases() {
    testCase1();
    testCase2();
    testCase3();
    testCase4();
}

int main() {
    testAllCases();
}