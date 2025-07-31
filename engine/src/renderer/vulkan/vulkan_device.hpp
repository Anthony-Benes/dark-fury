#pragma once

#include <defines.hpp>
#include <containers/darray.hpp>
#include <vulkan/vulkan.h>

struct vulkan_context;

struct vulkan_swapchain_support_info {
    VkSurfaceCapabilitiesKHR capabilities{};
    u32 format_count                = 0;
    VkSurfaceFormatKHR* formats     = nullptr;
    u32 present_mode_count          = 0;
    VkPresentModeKHR* present_modes = nullptr;
};

namespace Vulkan {
class Device {
  public:
    struct Requirements {
        b8 graphics           = false;
        b8 present            = false;
        b8 compute            = false;
        b8 transfer           = false;
        b8 sampler_anisotropy = false;
        b8 discrete_gpu       = false;
        Engine::DArray<const char*> device_extension_names;

        Requirements() = default;
    };

    struct QueueFamilyInfo {
        i32 graphics_index = -1;
        i32 present_index  = -1;
        i32 compute_index  = -1;
        i32 transfer_index = -1;
    };

  public:
    static Device& Get();

    static b8 Create(vulkan_context* context);
    static void Destroy(vulkan_context* context);
    static void QuerySwapchainSupport(VkPhysicalDevice physical_device, VkSurfaceKHR surface);

    const vulkan_swapchain_support_info& GetSwapchainSupport() const { return mSwapchainSupport; }

    VkPhysicalDevice physical = VK_NULL_HANDLE;
    VkDevice logical          = VK_NULL_HANDLE;
    QueueFamilyInfo mQueueFamily{};
    VkPhysicalDeviceProperties mProperties{};
    VkPhysicalDeviceFeatures mFeatures{};
    VkPhysicalDeviceMemoryProperties mMemory{};

  private:
    Device();
    ~Device();
    // prevent copying, enforce singleton
    Device(const Device&)            = delete;
    Device& operator=(const Device&) = delete;

    b8 select_physical_device();
    b8 physical_device_meets_requirements(VkPhysicalDevice device, VkSurfaceKHR surface,
                                          VkPhysicalDeviceProperties properties,
                                          VkPhysicalDeviceFeatures features,
                                          VkPhysicalDeviceMemoryProperties memory);

    vulkan_swapchain_support_info mSwapchainSupport{};
    Requirements mRequirements{};
    vulkan_context* mContext = nullptr;
};
}  // namespace Vulkan
