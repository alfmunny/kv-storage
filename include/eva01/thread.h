#pragma once

#include <pthread.h>
#include <functional>
#include <memory>
#include <string>
#include <condition_variable>
#include <mutex>
#include <eva01/util.h>
#include <eva01/noncopyable.h>
#include <eva01/semaphore.h>

namespace eva01 {

class Thread : NonCopyable {

public:
    using ptr = std::shared_ptr<Thread>;

    Thread(std::function<void()> cb, const std::string& name);

    inline pid_t getId() const { return m_id; }
    inline std::string getName() const { return m_name; }

    ~Thread();

    void join();
    static Thread* GetThis();
    static const std::string& GetName();
    static void SetName(const std::string& name);


private:
    pthread_t m_thread = 0;
    pid_t m_id = -1;
    std::function<void()> m_cb;
    std::string m_name;
    Semaphore m_semaphore;

private:
    static void* run(void* arg);
};


}
