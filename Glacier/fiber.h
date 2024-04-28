#pragma once

#include "thread.h"
#include <functional>
#include <memory>
#include <ucontext.h>

namespace Glacier {

class Fiber : public std::enable_shared_from_this<Fiber>
{
    friend class Scheduler;

public:
    using ptr = std::shared_ptr<Fiber>;

    enum State {
        INIT,
        HOLD,
        EXEC,
        TERM,
        READY,
        EXCEPT
    };

private:
    Fiber();

public:
    Fiber(std::function<void()> cb, size_t stackSize = 0, bool use_caller = false);
    ~Fiber();

    // 重置协程函数，并重置状态
    void reset(std::function<void()> cb);
    // 切换到当前协程执行
    void swapIn();
    // 切换到后台执行
    void swapOut();

    void call();
    void back();

    uint64_t getId() const { return m_id; }

    State getState() const { return m_state; }

public:
    // 设置当前协程
    static void SetThis(Fiber* f);
    // 返回当前协程
    static Fiber::ptr GetThis();
    // 协程切换到后台，并设置为Ready状态
    static void YelidToReady();
    // 协程切换到后台，并设置为Hold状态
    static void YelidToHold();
    // 总协程数
    static uint64_t TotalFibers();

    static void MainFunc();
    static void CallerMainFunc();
    static uint64_t GetFiberId();

private:
    uint64_t m_id = 0;
    uint64_t m_stackSize = 0;
    State m_state = INIT;
    ucontext_t m_ctx;
    void* m_stack = nullptr;

    std::function<void()> m_cb;
};

} // namespace Glacier
