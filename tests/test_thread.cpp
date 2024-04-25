#include "Glacier.h"
#include <unistd.h>

Glacier::Logger::ptr g_logger = GLACIER_LOG_ROOT();

int count = 0;
// Glacier::RWMutex s_mutex;
Glacier::Mutex s_mutex;

void fun1() {
    GLACIER_LOG_INFO(g_logger) << "name: " << Glacier::Thread::GetName()
                               << " this.name: " << Glacier::Thread::GetThis()->getName()
                               << " id: " << Glacier::GetThreadId()
                               << " this.id: " << Glacier::Thread::GetThis()->getId();

    for (int i = 0; i < 100000; ++i) {
        Glacier::Mutex::Lock lock(s_mutex);
        ++count;
    }
}

void fun2() {
    while (true) {
        GLACIER_LOG_INFO(g_logger) << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    }
}

void fun3() {
    while (true) {
        GLACIER_LOG_INFO(g_logger) << "=========================================";
    }
}

int main(int argc, char** argv) {
    GLACIER_LOG_INFO(g_logger) << "thread test begin";
    YAML::Node root = YAML::LoadFile("/home/penguin/code/Glacier/bin/conf/log2.yml");
    Glacier::Config::LoadFromYaml(root);

    std::vector<Glacier::Thread::ptr> thrs;

    for (int i = 0; i < 2; ++i) {
        Glacier::Thread::ptr thr(new Glacier::Thread(fun2, "name_" + std::to_string(i * 2)));
        Glacier::Thread::ptr thr2(new Glacier::Thread(fun3, "name_" + std::to_string(i * 2 + 1)));
        thrs.push_back(thr);
        thrs.push_back(thr2);
    }

    for (size_t i = 0; i < thrs.size(); ++i) {
        thrs[i]->join();
    }
    GLACIER_LOG_INFO(g_logger) << "thread test end";
    GLACIER_LOG_INFO(g_logger) << "count=" << count;

    return 0;
}
