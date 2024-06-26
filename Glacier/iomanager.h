#pragma once

#include "scheduler.h"

namespace Glacier {

class IOManager : public Scheduler
{
public:
    using ptr = std::shared_ptr<IOManager>;
    using RWMutexType = RWMutex;

    enum Event {
        NONE = 0x0,
        READ = 0x1,
        WRITE = 0x4,
    };

private:
    struct FdContext {
        using MutexType = Mutex;
        struct EventContext {
            Scheduler* scheduler;     // 事件执行的 scheduler
            Fiber::ptr fiber;         // 事件协程
            std::function<void()> cb; // 事件的回调函数
        };

        EventContext read;   // 读事件
        EventContext write;  // 写事件
        int fd = 0;          // 事件关联的句柄
        Event events = NONE; // 已经注册的事件
        MutexType mutex;
    };

public:
    IOManager(size_t threads = 1, bool use_caller = true, const std::string& name = "");
    ~IOManager();

    // 1 success, 0 retry, -1 error
    int addEvent(int fd, Event event, std::function<void()> cb = nullptr);
    bool delEvent(int fd, Event event);
    bool cancelEvent(int fd, Event event);

    bool cancelAll(int fd);

    static IOManager* GetThis();
    static void SetThis(IOManager* manager);
    static IOManager* GetAsThis();

protected:
    void tickle() override;
    bool stopping() override;
    void idle() override;

    void contextResize(size_t size);

private:
    int m_epfd = 0;
    int m_tickleFds[2];

    std::atomic<size_t> m_pendingEventCount = {0};
    RWMutexType m_mutex;
    std::vector<FdContext*> m_fdContexts;
};

} // namespace Glacier
