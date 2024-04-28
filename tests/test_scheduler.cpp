#include "Glacier.h"

static Glacier::Logger::ptr g_logger = GLACIER_LOG_ROOT();

void test_fiber() {
    static int s_count = 5;
    GLACIER_LOG_INFO(g_logger) << "test_fiber s_count=" << s_count;

    sleep(1);

    if (--s_count >= 0) {
        Glacier::Scheduler::GetThis()->schedule(&test_fiber);
    }
}

int main(int argc, char** argv) {
    GLACIER_LOG_INFO(g_logger) << "main";
    Glacier::Scheduler sc(3, false, "test");
    sc.start();
    sleep(2);
    GLACIER_LOG_INFO(g_logger) << "schedule";
    sc.schedule(&test_fiber);
    sc.stop();
    GLACIER_LOG_INFO(g_logger) << "over";
    return 0;
}
