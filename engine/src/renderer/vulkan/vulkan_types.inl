#pragma once

#include <defines.hpp>
#include <core/asserts.hpp>
#include <vulkan/vulkan.h>
#include <renderer/vulkan/vulkan_device.hpp>

#define VK_CHECK(expr)                                                                             \
    { DF_ASSERT(expr == VK_SUCCESS); }

struct vulkan_context {
    VkInstance instance;
    VkAllocationCallbacks* allocator;
    VkSurfaceKHR surface;
#if defined(_DEBUG)
    VkDebugUtilsMessengerEXT debug_messenger;
#endif
    Vulkan::Device* device;
};
