
#include "../Glacier/log.h"
#include "../Glacier/util.h"

// 定义一个日志器(这里使用的是root)
static Glacier::Logger::ptr g_logger = GLACIER_LOG_ROOT();

int main(int argc, char** argv) {
    // 使用流式风格写日志
    GLACIER_LOG_DEBUG(g_logger) << "hello logger stream";
    // 使用格式化写日志
    GLACIER_LOG_FMT_INFO(g_logger, "%s", "hello logger format");
    return 0;
}
