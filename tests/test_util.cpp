#include "Glacier.h"
#include <cassert>

Glacier::Logger::ptr g_logger = GLACIER_LOG_ROOT();

void test_assert() {
    GLACIER_LOG_INFO(g_logger) << Glacier::BacktraceToString(10);
    GLACIER_ASSERT2(0 == 1, "assert test");
}

int main(int argc, char** argv) {
    test_assert();

    return 0;
}
