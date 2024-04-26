#include "Glacier.h"

Glacier::Logger::ptr g_logger = GLACIER_LOG_ROOT();

void run_in_fiber() {
    GLACIER_LOG_INFO(g_logger) << "run_in_fiber begin";
    Glacier::Fiber::YelidToHold();
    GLACIER_LOG_INFO(g_logger) << "run_in_fiber end";
    Glacier::Fiber::YelidToHold();
}

void test_fiber() {
    GLACIER_LOG_INFO(g_logger) << "main begin -1";
    {
        Glacier::Fiber::GetThis();
        GLACIER_LOG_INFO(g_logger) << "main begin";
        Glacier::Fiber::ptr fiber(new Glacier::Fiber(run_in_fiber));
        fiber->swapIn();
        GLACIER_LOG_INFO(g_logger) << "main after swapIn";
        fiber->swapIn();
        GLACIER_LOG_INFO(g_logger) << "main after end";
        fiber->swapIn();
    }

    GLACIER_LOG_INFO(g_logger) << "main after end2";
}

int main(int argc, char** argv) {
    Glacier::Thread::SetName("main");

    std::vector<Glacier::Thread::ptr> thrs;
    for (int i = 0; i < 3; ++i) {
        thrs.push_back(Glacier::Thread::ptr(new Glacier::Thread(&test_fiber, "name_" + std::to_string(i))));
    }

    for (auto i : thrs) {
        i->join();
    }

    return 0;
}
