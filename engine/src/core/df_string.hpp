#pragma once

#include <defines.hpp>
#include <core/df_memory.hpp>

#include <string.h>

namespace Engine {

DF_API char* string_dup(const char* str);
DF_API u64 str_len(const char* str);
DF_API b8 strs_eq(const char* str0, const char* str1);

inline char* string_dup(const char* str) {
    auto length = str_len(str);
    char* copy = (char*)Memory::df_allocate(length + 1, Memory::Tag::STRING);
    Memory::df_copy_memory(copy, str, length + 1);
    return copy;
}

inline u64 str_len(const char *str) {
    return strlen(str);
}

inline b8 strs_eq(const char* str0, const char* str1) {
    return strcmp(str0, str1) == 0;
}

} // namespace Engine
