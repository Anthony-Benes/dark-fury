#pragma once

#include <defines.hpp>

namespace Engine::Memory {
enum class Tag {
    UNKNOWN,
    ARRAY,
    DARRAY,
    DICT,
    RING_QUEUE,
    BST,
    STRING,
    APPLICATION,
    JOB,
    TEXTURE,
    MATERIAL_INSTANCE,
    RENDERER,
    GAME,
    TRANSFORM,
    ENTITY,
    ENTITY_NODE,
    SCENE,
};
static const u64 MaxTags = 17;

DF_API void Initialize();

DF_API void Shutdown();

DF_API void* df_allocate(u64 size, Tag tag);

DF_API void df_free(void* block, u64 size, Tag tag);

DF_API void* df_zero_memory(void* block, u64 size);

DF_API void* df_copy_memory(void* dest, const void* source, u64 size);

DF_API void* df_set_memory(void* dest, i32 value, u64 size);

DF_API char* get_memory_usage_str();

} // namespace Memory
