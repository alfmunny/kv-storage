#pragma once
#include <pthread.h>
#include <assert.h>
#include <eva01/noncopyable.h>
#include <eva01/util.h>

namespace eva01 {
class Mutex : public NonCopyable {
public:
    Mutex() : m_thread(0) {
        pthread_mutex_init(&m_mutex, NULL);
    }

    ~Mutex() {
        assert(m_thread == 0);
        pthread_mutex_destroy(&m_mutex);
    }

    void lock() { 
        pthread_mutex_lock(&m_mutex); 
        m_thread = GetThreadId();
    }

    void unlock() { 
        m_thread = 0;
        pthread_mutex_unlock(&m_mutex); 
    }

    bool isLockedByThisThread()
    {
        return m_thread == GetThreadId();
    }

private:
    pthread_mutex_t m_mutex;
    pid_t m_thread;

};

template <typename T>
class MutexGuard {
public:
    MutexGuard(T& mtx) : m_mutex(mtx) {
        m_mutex.lock();
    }

    ~MutexGuard() {
        m_mutex.unlock();
    }

private:
    T& m_mutex;
};

class RWMutex : public NonCopyable {
public:
    RWMutex() {
        pthread_rwlock_init(&m_lock, NULL);
    }

    ~RWMutex() {
        pthread_rwlock_destroy(&m_lock);
    }

    void rdlock() { 
        pthread_rwlock_rdlock(&m_lock); 
    }

    void wrlock() {
        pthread_rwlock_wrlock(&m_lock);
    }

    void unlock() { 
        pthread_rwlock_unlock(&m_lock); 
    }

private:
    pthread_rwlock_t m_lock;
};

class MutexReadGuard {
public:
    MutexReadGuard(RWMutex& mtx) 
        : m_mutex(mtx) {
        m_mutex.rdlock();
    }

    ~MutexReadGuard() {
        m_mutex.unlock();
    }

private:
    RWMutex& m_mutex;

};

class MutexWriteGuard {
public:
    MutexWriteGuard(RWMutex& mtx) 
        : m_mutex(mtx) {
        m_mutex.wrlock();
    };

    ~MutexWriteGuard() {
        m_mutex.unlock();
    }

private:
    RWMutex& m_mutex;

};

}
