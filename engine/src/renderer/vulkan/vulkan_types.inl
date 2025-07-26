#pragma once

#include <defines.hpp>
#include <core/asserts.hpp>
#include <vulkan/vulkan.h>

#define VK_CHECK(expr)                                                                             \
    { DF_ASSERT(expr == VK_SUCCESS); }

typedef struct vulkan_context {
    VkInstance instance;
    VkAllocationCallbacks* allocator;
#if defined(_DEBUG)
    VkDebugUtilsMessengerEXT debug_messenger;
#endif
} vulkan_context;
