#pragma once

#include <cstdint>
#include <sched.h>
namespace Glacier {

pid_t GetThreadId();

uint32_t GetFiberId();

} // namespace Glacier
