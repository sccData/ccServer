#include "../Logging.h"
#include "../Thread.h"
#include <string>
#include <unistd.h>
#include <vector>
#include <memory>
#include <iostream>
using namespace std;

void threadFunc() {
    for(int i=0; i<100000; ++i) {
        LOG_INFO << i;
        LOG_WARN << i;
    }
}

void type_test() {
    cout << "---------type test----------" << endl;
    LOG_INFO << 0;
    LOG_INFO << 1234567890123;
    LOG_INFO << 1.0f;
    LOG_INFO << 3.1415926;
    LOG_INFO << (short) 1;
    LOG_INFO << (long long) 1;
    LOG_INFO << (unsigned int) 1;
    LOG_INFO << (unsigned long) 1;
    LOG_INFO << (long double) 1.65555;
    LOG_INFO << (unsigned long long) 1;
    LOG_INFO << 'c';
    LOG_INFO << "avfgdf";
    LOG_INFO << string("I am scc");
}

void stressing_single_thread() {
    cout << "-------stressing test single thread----------" << endl;
    for(int i=0; i<100000; ++i) {
        LOG_INFO << i;
    }
}

void stressing_multi_threads(int threadNum = 4) {
    cout << "--------stressing test multi thread----------" << endl;
    vector<shared_ptr<Thread>> vsp;
    for(int i=0; i<threadNum; ++i) {
        shared_ptr<Thread> tmp(new Thread(threadFunc, "thestFunc"));
        vsp.push_back(tmp);
    }

    for(int i=0; i<threadNum; ++i) {
        vsp[i]->start();
    }

    sleep(3);
}

void other() {
    cout << "-----------other test-------------" << endl;
    LOG_INFO << "fdsfsdf" << 'f' << 0 << 3.666 << string("I am scc");
}

int main() {
    type_test();
    sleep(3);

    stressing_single_thread();
    sleep(3);

    stressing_multi_threads();
    sleep(3);
    return 0;
}