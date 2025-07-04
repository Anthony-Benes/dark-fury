#pragma once

#include "defines.hpp"

#ifndef DF_ASSERTIONS_ENABLED
#define DF_ASSERTIONS_ENABLED 1
#endif

#if defined(DF_ASSERTIONS_ENABLED) && DF_ASSERTIONS_ENABLED == 1
#if _MSC_VER
#include <intrin.h>
#define debugBreak() __debugbreak()
#else
#define debugBreak() __builtin_trap()
#endif

DF_API void report_assertion_failure(const char* expression, const char* message, const char* file, i32 line);

#define DF_ASSERT(expr)                                                 \
    {                                                                   \
        if (expr) {                                                     \
        } else {                                                        \
            report_assertion_failure(#expr, "", __FILE__, __LINE__);    \
            debugBreak();                                               \
        }                                                               \
    }

#define DF_ASSERT_MSG(expr, message)                                        \
    {                                                                       \
        if (expr) {                                                         \
        } else {                                                            \
            report_assertion_failure(#expr, message, __FILE__, __LINE__);   \
            debugBreak();                                                   \
        }                                                                   \
    }

#ifdef _DEBUG
#define DF_ASSERT_DEBUG(expr)                                           \
    {                                                                   \
        if (expr) {                                                     \
        } else {                                                        \
            report_assertion_failure(#expr, "", __FILE__, __LINE__);    \
            debugBreak();                                               \
        }                                                               \
    }
#else
#define DF_ASSERT_DEBUG(expr)
#endif

#else
#define DF_ASSERT(expr)
#define DF_ASSERT_MSG(expr, message)
#define DF_ASSERT_DEBUG(expr)
#endif
