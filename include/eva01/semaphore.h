#pragma once

#include <eva01/util.h>
#include <eva01/noncopyable.h>
#include <semaphore.h>
#include <cstdint>

namespace eva01 {

class Semaphore : public NonCopyable {
public:
    Semaphore(uint32_t count = 0);
    ~Semaphore();

    void wait();
    void notify();

private:
    sem_t m_semaphore;
};

}
