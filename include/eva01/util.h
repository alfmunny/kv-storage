#pragma once

#include <sys/syscall.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <cstdint>
#include <vector>
#include <string>

namespace eva01 {

pid_t GetThreadId();

void SleepMs(int ms);

uint64_t GetCurrentMs();

void Backtrace(std::vector<std::string>& bt, int size, int skip = 1);

std::string BacktraceToString(int size, int skip, const std::string& prefix);

}
