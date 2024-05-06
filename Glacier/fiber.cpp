#include "fiber.h"
#include "config.h"
#include "log.h"
#include "macro.h"
#include "scheduler.h"
#include <atomic>

namespace Glacier {

static Logger::ptr g_logger = GLACIER_LOG_NAME("system");

static std::atomic<uint64_t> s_fiber_id{0};
static std::atomic<uint64_t> s_fiber_count{0};

static thread_local Fiber* t_fiber = nullptr;
static thread_local Fiber::ptr t_threadFiber = nullptr;

static ConfigVar<uint32_t>::ptr g_fiber_stack_size = Glacier::Config::Lookup<uint32_t>("fiber.stack_size", 1024 * 1024, "fiber stack size");

// 创建协程栈
class MallocStackAllocator
{
public:
    static void* Alloc(size_t size) {
        return malloc(size);
    }

    static void Dealloc(void* vp, size_t size) {
        free(vp);
    }
};

using StackAllocator = MallocStackAllocator;

uint64_t Fiber::GetFiberId() {
    if (t_fiber) {
        return t_fiber->getId();
    }

    return 0;
}

Fiber::Fiber() {
    m_state = EXEC;
    SetThis(this);

    if (getcontext(&m_ctx)) {
        GLACIER_ASSERT2(false, "getcontext");
    }

    ++s_fiber_count;

    GLACIER_LOG_DEBUG(g_logger) << "Fiber::Fiber";
}

Fiber::Fiber(std::function<void()> cb, size_t stackSize, bool use_caller)
    : m_id(++s_fiber_id)
    , m_cb(cb) {
    ++s_fiber_count;
    m_stackSize = stackSize ? stackSize : g_fiber_stack_size->getValue();

    m_stack = StackAllocator::Alloc(m_stackSize);
    if (getcontext(&m_ctx) == -1) {
        GLACIER_ASSERT2(false, "getcontext");
    }

    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stackSize;

    if (!use_caller) {
        makecontext(&m_ctx, &Fiber::MainFunc, 0);
    } else {
        makecontext(&m_ctx, &Fiber::CallerMainFunc, 0);
    }

    GLACIER_LOG_DEBUG(g_logger) << "Fiber::Fiber id=" << m_id;
}

Fiber::~Fiber() {
    s_fiber_count--;
    if (m_stack) {
        GLACIER_ASSERT(m_state == TERM || m_state == EXCEPT || m_state == INIT);

        StackAllocator::Dealloc(m_stack, m_stackSize);
    } else {
        GLACIER_ASSERT(!m_cb);
        GLACIER_ASSERT(m_state == EXEC);

        Fiber* cur = t_fiber;
        if (cur == this) {
            SetThis(nullptr);
        }
    }
    GLACIER_LOG_DEBUG(g_logger) << "Fiber::~Fiber id=" << m_id;
}

// 重置协程函数，并重置状态
void Fiber::reset(std::function<void()> cb) {
    GLACIER_ASSERT(m_stack);
    GLACIER_ASSERT(m_state == TERM || m_state == EXCEPT || m_state == INIT);

    m_cb = cb;
    if (getcontext(&m_ctx) == -1) {
        GLACIER_ASSERT2(false, "getcontext");
    }

    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stackSize;

    makecontext(&m_ctx, &Fiber::MainFunc, 0);
    m_state = INIT;
}

// 从协程主协程切换到当前协程执行
void Fiber::call() {
    SetThis(this);
    m_state = EXEC;
    if (swapcontext(&t_threadFiber->m_ctx, &m_ctx)) {
        GLACIER_ASSERT2(false, "swapcontext");
    }
}

// 从当前协程切换到主协程
void Fiber::back() {
    SetThis(t_threadFiber.get());
    if (swapcontext(&m_ctx, &t_threadFiber->m_ctx)) {
        GLACIER_ASSERT2(false, "swapcontext");
    }
}

// 从调度器的主协程切换到当前协程执行
void Fiber::swapIn() {
    SetThis(this);
    GLACIER_ASSERT(m_state != EXEC);
    m_state = EXEC;

    if (swapcontext(&Scheduler::GetMainFiber()->m_ctx, &m_ctx)) {
        GLACIER_ASSERT2(false, "swapcontext");
    }
}

// 从当前协程切换到调度器的主协程
void Fiber::swapOut() {
    SetThis(Scheduler::GetMainFiber());

    if (swapcontext(&m_ctx, &Scheduler::GetMainFiber()->m_ctx)) {
        GLACIER_ASSERT2(false, "swapcontext");
    }
}

// 设置当前协程
void Fiber::SetThis(Fiber* f) {
    t_fiber = f;
}

// 返回当前协程并获得主协程
Fiber::ptr Fiber::GetThis() {
    if (t_fiber) {
        return t_fiber->shared_from_this();
    }

    Fiber::ptr main_fiber(new Fiber);
    GLACIER_ASSERT(t_fiber == main_fiber.get());
    t_threadFiber = main_fiber;
    return t_fiber->shared_from_this();
}

// 协程切换到后台，并设置为Ready状态
void Fiber::YelidToReady() {
    Fiber::ptr cur = GetThis();
    GLACIER_ASSERT(cur->m_state == EXEC);
    cur->m_state = READY;
    cur->swapOut();
}

// 协程切换到后台，并设置为Hold状态
void Fiber::YelidToHold() {
    Fiber::ptr cur = GetThis();
    GLACIER_ASSERT(cur->m_state == EXEC);
    cur->m_state = HOLD;
    cur->swapOut();
}

// 总协程数
uint64_t Fiber::TotalFibers() {
    return s_fiber_count;
}

// 协程执行函数
void Fiber::MainFunc() {
    Fiber::ptr cur = GetThis();
    GLACIER_ASSERT(cur);
    try {
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = TERM;
    } catch (std::exception& e) {
        cur->m_state = EXCEPT;
        GLACIER_LOG_ERROR(g_logger) << "Fiber Except: " << e.what()
                                    << " fiber_id=" << cur->getId()
                                    << std::endl
                                    << Glacier::BacktraceToString();

    } catch (...) {
        cur->m_state = EXCEPT;
        GLACIER_LOG_ERROR(g_logger) << "Fiber Except"
                                    << " fiber_id=" << cur->getId()
                                    << std::endl
                                    << Glacier::BacktraceToString();
    }

    auto raw_ptr = cur.get();
    cur.reset();
    raw_ptr->swapOut();

    GLACIER_ASSERT2(false, "never reach fiber_id=" + std::to_string(raw_ptr->getId()));
}

void Fiber::CallerMainFunc() {
    Fiber::ptr cur = GetThis();
    GLACIER_ASSERT(cur);
    try {
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = TERM;
    } catch (std::exception& e) {
        cur->m_state = EXCEPT;
        GLACIER_LOG_ERROR(g_logger) << "Fiber Except: " << e.what()
                                    << " fiber_id=" << cur->getId()
                                    << std::endl
                                    << Glacier::BacktraceToString();
    } catch (...) {
        cur->m_state = EXCEPT;
        GLACIER_LOG_ERROR(g_logger) << "Fiber Except"
                                    << " fiber_id=" << cur->getId()
                                    << std::endl
                                    << Glacier::BacktraceToString();
    }
    auto raw_ptr = cur.get();
    cur.reset();
    raw_ptr->back();
    GLACIER_ASSERT2(false, "never reach fiber_id=" + std::to_string(raw_ptr->getId()));
}

} // namespace Glacier
