#pragma once

#include "util.h"
#include <cassert>
#include <string>

#define GLACIER_ASSERT(x)                                                                    \
    if (!(x)) {                                                                              \
        GLACIER_LOG_ERROR(GLACIER_LOG_ROOT()) << "ASSERTION: " #x                            \
                                              << "\nbacktrace:\n"                            \
                                              << Glacier::BacktraceToString(100, 2, "    "); \
        assert(x);                                                                           \
    }

#define GLACIER_ASSERT2(x, w)                                                                \
    if (!(x)) {                                                                              \
        GLACIER_LOG_ERROR(GLACIER_LOG_ROOT()) << "ASSERTION: " #x << "\n"                    \
                                              << w                                           \
                                              << "\nbacktrace:\n"                            \
                                              << Glacier::BacktraceToString(100, 2, "    "); \
        assert(x);                                                                           \
    }
