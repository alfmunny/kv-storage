#pragma once

#include <eva01/fiber.h>
#include <eva01/noncopyable.h>
#include <eva01/thread.h>
#include <eva01/mutex.h>
#include <eva01/macro.h>
#include <eva01/timer.h>
#include <vector>
#include <list>
#include <functional>
#include <atomic>
#include <sys/epoll.h>

namespace eva01 {

class Scheduler : public TimerManager {

public:
    using ptr = std::shared_ptr<Scheduler>;
    using MutexType = Mutex;

    Scheduler(int threads = 1, const std::string& name = "main");
    virtual ~Scheduler();

public:
    static Scheduler* GetThis();
    static Fiber* GetMainFiber();

    const std::string& getName() const { return m_name; }
    void start();
    void stop();

    template <typename FiberOrFunc>
    void schedule(FiberOrFunc f, int thr = -1) {
        MutexGuard<MutexType> lk(m_mutex);
        bool need_tickle = m_tasks.empty();
        m_tasks.emplace_back(std::move(f), thr);
        if (need_tickle) {
            tickle();
        }
    }

    template <typename Iterator>
    void schedule(Iterator a, Iterator b, int thr = -1) {
        MutexGuard<MutexType> lk(m_mutex);
        bool need_tickle = m_tasks.empty();
        for (auto it = a; it != b; ++it) {
            m_tasks.emplace_back(std::move(*it), thr);
        }
        if (need_tickle) {
            tickle();
        }
    }

protected:

    void onFirstTimerChanged() override;
    virtual bool scheduleEvent(epoll_event& event) { return true; };

private:
    void run();
    void tickle();
    void idle();
    bool shouldStop();

private:
struct Task {
    Fiber::ptr fiber;
    std::function<void()> func;
    int thread_id;

    Task(Fiber::ptr fb, int thr_id)
        :fiber(std::move(fb)), thread_id(thr_id) {
    }

    Task(std::function<void()> fc, int thr_id)
        :func(std::move(fc)), thread_id(thr_id) {
    } 

    Task() 
        : thread_id(-1) {
    }

    void reset() {
        fiber = nullptr;
        func = nullptr;
        thread_id = -1;
    }
};

private:
    MutexType m_mutex;
    Fiber::ptr m_main_fiber;
    std::string m_name;
    std::vector<Thread::ptr> m_threads;
    std::list<Task> m_tasks;

    size_t m_thread_count = 0;
    bool m_stopped = true;
    bool m_stopping = false;
    bool m_auto_stop = false;

    std::atomic<size_t> m_active_threads{ 0 };
    std::atomic<size_t> m_idle_threads{ 0 };
    int m_root_thread = 0;
    int m_tickle_fds[2];

protected:
    int m_epfd = 0;
    std::atomic<size_t> m_pending_count { 0 };
};

}
