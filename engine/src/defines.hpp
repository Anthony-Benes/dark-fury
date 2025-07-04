#pragma once

// Unsigned int types
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
// Signed int types
typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;
// Floating point types
typedef float f32;
typedef double f64;
// Boolean types
typedef int b32;
typedef char b8;

// Properly define static assertions
#if defined(__clang__) || defined(__gcc__)
#define STATIC_ASSERT(expr, msg) _Static_assert(expr, msg)
#else
#define STATIC_ASSERT(expr, msg) static_assert(expr, msg)
#endif

// Ensure all types are of the correct size
namespace {
STATIC_ASSERT(sizeof(u8) == 1, "u8 should be 1 byte");
STATIC_ASSERT(sizeof(u16) == 2, "u16 should be 2 bytes");
STATIC_ASSERT(sizeof(u32) == 4, "u32 should be 4 bytes");
STATIC_ASSERT(sizeof(u64) == 8, "u64 should be 8 bytes");
STATIC_ASSERT(sizeof(i8) == 1, "i8 should be 1 byte");
STATIC_ASSERT(sizeof(i16) == 2, "i16 should be 2 bytes");
STATIC_ASSERT(sizeof(i32) == 4, "i32 should be 4 bytes");
STATIC_ASSERT(sizeof(i64) == 8, "i64 should be 8 bytes");
STATIC_ASSERT(sizeof(f32) == 4, "f32 should be 4 bytes");
STATIC_ASSERT(sizeof(f64) == 8, "f64 should be 8 bytes");
}

#define TRUE 1
#define FALSE 0

// Platform detection
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define DF_PLATFORM_WINDOWS 1
#ifndef _WIN64
#error "64-bit is required on Windows!"
#endif
#elif defined(__linux__) || defined(__gnu_linux__)
// Linux OS
#define DF_PLATFORM_LINUX 1
#if defined(__ANDROID__)
#define DF_PLATFORM_ANDROID 1
#endif
#elif defined(__unix__)
// Catch anything not caught by the above
#define DF_PLATFORM_UNIX 1
#elif defined(_POSIX_VERSION)
// Posix
#define DF_PLATFORM_POSIX 1
#elif defined(__APPLE__) && __APPLE__
// Apple platforms
#define DF_PLATFORM_APPLE 1
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR
// iOS Simulator
#define DF_PLATFORM_IOS 1
#define DF_PLATFORM_IOS_SIMULATOR 1
#elif TARGET_OS_IPHONE
#define DF_PLATFORM_IOS 1
// iOS device
#elif TARGET_OS_MAC
// Other kinds of Mac OS
#else
#error "Unknown Apple platform"
#endif
#else
#error "Unknown platform!"
#endif

#ifdef DF_EXPORT
// Exports
#ifdef _MSC_VER
#define DF_API __declspec(dllexport)
#else
#define DF_API __attribute__((visibility("default")))
#endif
#else
// Imports
#ifdef _MSC_VER
#define DF_API __declspec(dllimport)
#else
#define DF_API
#endif
#endif
