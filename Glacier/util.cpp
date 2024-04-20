#include "util.h"
#include "log.h"
#include <cstdlib>
#include <sys/syscall.h>
#include <unistd.h>

namespace Glacier {

static Glacier::Logger::ptr g_logger = GLACIER_LOG_NAME("system");

pid_t GetThreadId() {
    return syscall(SYS_gettid);
}

uint32_t GetFiberId() {
    return 0;
}

} // namespace Glacier
