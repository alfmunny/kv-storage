#pragma once
#include <eva01/scheduler.h>
#include <eva01/mutex.h>
#include <atomic>
#include <bits/stdint-uintn.h>
#include <functional>
#include <stdexcept>

namespace eva01 {

class IOManager : public Scheduler {

public:
    using ptr = std::shared_ptr<IOManager>;
    using MutexType = RWMutex;

    enum Event {
        NONE = 0x0,
        READ = 0x1,
        WRITE = 0x4,
    };

    IOManager(int threads, const std::string& name = "iomanager");
    ~IOManager();
private:
    struct FdContext {
        struct EventContext {
            Scheduler* scheduler = nullptr;
            Fiber::ptr fiber;
            std::function<void()> func;
        };

        EventContext& getContext(Event event) {
            switch(event) {
                case READ:
                    return read;
                case WRITE:
                    return write;
                default:
                    ASSERT(false);
            }
            throw std::invalid_argument("getContext invalid event");
        };

        void resetContext(Event event) {
            auto event_ctx = getContext(event);
            event_ctx.fiber = nullptr;
            event_ctx.func = nullptr;
            event_ctx.scheduler = nullptr;
        };

        void scheduleEvent(Event event) {
            ASSERT(events & event);
            events = (Event)(events & ~event);
            auto event_ctx = getContext(event);
            if (event_ctx.func) {
                event_ctx.scheduler->schedule(event_ctx.func);
            } 
            else if (event_ctx.fiber) {
                event_ctx.scheduler->schedule(event_ctx.fiber);
            } else {
                ASSERT(false); // either func or fiber must exist
            }

            event_ctx.scheduler = nullptr;
        }

        int fd = 0;
        EventContext read;
        EventContext write;
        Event events;
        Mutex mtx;
    };

private:
    void resizeContexts(size_t size);

public:
    bool scheduleEvent(epoll_event& event) override;
    bool addEvent(int fd, Event event, std::function<void()> func = nullptr);
    bool cancelEvent(int fd, Event event);
    bool delEvent(int fd, Event event);
    static IOManager* GetThis();

private:
    std::vector<FdContext*> m_fd_contexts;
    MutexType m_mutex;
};

} 
