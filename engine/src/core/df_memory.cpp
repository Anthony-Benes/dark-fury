#include <core/df_memory.hpp>
#include <core/df_string.hpp>
#include <core/logger.hpp>
#include <platform/platform.hpp>

#include <stdio.h>

namespace Engine::Memory {
struct memory_stats {
    u64 total_allocated;
    u64 tagged_allocations[MaxTags];
};
static struct memory_stats stats;

static const char* memory_tag_strings[MaxTags] = {
    "UNKNOWN    ", "ARRAY      ", "DARRAY     ", "DICT       ", "RING_QUEUE ", "BST        ",
    "STRING     ", "APPLICATION", "JOB        ", "TEXTURE    ", "MAT_INST   ", "RENDERER   ",
    "GAME       ", "TRANSFORM  ", "ENTITY     ", "ENTITY_NODE", "SCENE      "
};

void Initialize() { platform_zero_memory(&stats, sizeof(stats)); }

void Shutdown() {}

void* df_allocate(u64 size, Tag tag) {
    if ( tag == Tag::UNKNOWN ) {
        Log::Warn("df_allocate called using Memory::Tag::UNKNOWN. Re-class this allocation.");
    }
    stats.total_allocated += size;
    stats.tagged_allocations[static_cast<u16>(tag)] += size;
    void* block = platform_allocate(size, false);
    platform_zero_memory(block, size);
    return block;
}

void df_free(void* block, u64 size, Tag tag) {
    if ( tag == Tag::UNKNOWN ) {
        Log::Warn("df_free called using Memory::Tag::UNKNOWN. Re-class this allocation.");
    }
    stats.total_allocated -= size;
    stats.tagged_allocations[static_cast<u16>(tag)] -= size;
    platform_free(block, false);
}

void* df_zero_memory(void* block, u64 size) { return platform_zero_memory(block, size); }

void* df_copy_memory(void* dest, const void* source, u64 size) {
    return platform_copy_memory(dest, source, size);
}

void* df_set_memory(void* dest, i32 value, u64 size) {
    return platform_set_memory(dest, value, size);
}

char* get_memory_usage_str() {
    const u64 kib     = 1024;
    const u64 mib     = 1024 * kib;
    const u64 gib     = 1024 * mib;
    char buffer[8000] = "System memory use (tagged):\n";
    u64 offset        = strlen(buffer);
    for ( u32 i = 0; i < MaxTags; i++ ) {
        char unit[4] = "XiB";
        float amount = 1.0f;
        if ( stats.tagged_allocations[i] >= gib ) {
            unit[0] = 'G';
            amount  = stats.tagged_allocations[i] / (float)gib;
        } else if ( stats.tagged_allocations[i] >= mib ) {
            unit[0] = 'M';
            amount  = stats.tagged_allocations[i] / (float)mib;
        } else if ( stats.tagged_allocations[i] >= kib ) {
            unit[0] = 'K';
            amount  = stats.tagged_allocations[i] / (float)kib;
        } else {
            unit[0] = 'B';
            unit[1] = 0;
            amount  = (float)stats.tagged_allocations[i];
        }
        i32 length =
          snprintf(buffer + offset, 7999, "  %s: %.2f%s\n", memory_tag_strings[i], amount, unit);
        offset += length;
    }
    char* out_str = string_dup(buffer);
    return out_str;
}

}  // namespace Engine::Memory
