#include "../Glacier/log.h"
#include <iostream>

int main(int argc, char** argv) {
    Glacier::Logger::ptr logger(new Glacier::Logger);
    logger->addAppender(Glacier::LogAppender::ptr(new Glacier::StdoutLogAppender));

    Glacier::LogEvent::ptr event(new Glacier::LogEvent(__FILE__, __LINE__, 0, 1, 2, time(0)));
    event->getSS() << "hello sylar log";

    logger->log(Glacier::LogLevel::DEBUG, event);

    std::cout << "Hello Glacier log" << std::endl;

    return 0;
}
