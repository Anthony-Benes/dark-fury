#include "vulkan_device.hpp"

#include <core/df_memory.hpp>
#include <core/df_string.hpp>
#include <core/logger.hpp>
#include <renderer/vulkan/vulkan_types.inl>

namespace Vulkan {
Device& Device::Get() {
    static Device instance;
    return instance;
}

Device::Device() {}

Device::~Device() {}

b8 Device::Create(vulkan_context* context) {
    Device& device  = Get();
    device.mContext = context;
    if ( !device.select_physical_device() ) { return false; }

    return true;
}

void Device::Destroy(vulkan_context* context) {
    Device& device  = Get();
    device.logical  = VK_NULL_HANDLE;
    device.physical = VK_NULL_HANDLE;
    device.mContext = nullptr;
    context->device = nullptr;
}

void Device::QuerySwapchainSupport(VkPhysicalDevice physical_device, VkSurfaceKHR surface) {
    Device& device = Get();
    auto& support  = device.mSwapchainSupport;
    VK_CHECK(
      vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &support.capabilities));
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &support.format_count,
                                                  nullptr));
    if ( support.format_count != 0 ) {
        if ( !support.formats ) {
            support.formats = static_cast<VkSurfaceFormatKHR*>(Engine::Memory::df_allocate(
              sizeof(VkSurfaceFormatKHR) * support.format_count, Engine::Memory::Tag::RENDERER));
        }
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface,
                                                      &support.format_count, support.formats));
    }
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface,
                                                       &support.present_mode_count, nullptr));
    if ( support.present_mode_count != 0 ) {
        if ( !support.present_modes ) {
            support.present_modes = static_cast<VkPresentModeKHR*>(
              Engine::Memory::df_allocate(sizeof(VkPresentModeKHR) * support.present_mode_count,
                                          Engine::Memory::Tag::RENDERER));
        }
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
          physical_device, surface, &support.present_mode_count, support.present_modes));
    }
}

b8 Device::select_physical_device() {
    u32 physical_device_count = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(mContext->instance, &physical_device_count, nullptr));
    if ( physical_device_count == 0 ) {
        Engine::Log::Fatal("No devices which support Vulkan were found.");
        return false;
    }
    Engine::DArray<VkPhysicalDevice> physical_devices =
      Engine::DArray<VkPhysicalDevice>(physical_device_count);
    VK_CHECK(vkEnumeratePhysicalDevices(mContext->instance, &physical_device_count,
                                        physical_devices.data()));
    for ( u32 i = 0; i < physical_device_count; ++i ) {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physical_devices[i], &properties);
        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(physical_devices[i], &features);
        VkPhysicalDeviceMemoryProperties memory;
        vkGetPhysicalDeviceMemoryProperties(physical_devices[i], &memory);

        b8 result = physical_device_meets_requirements(physical_devices[i], mContext->surface,
                                                       properties, features, memory);
        if ( result ) {
            Engine::Log::Info("Selected device: '%s'.", properties.deviceName);
            switch ( properties.deviceType ) {
                default:
                case VK_PHYSICAL_DEVICE_TYPE_OTHER:
                    Engine::Log::Info("GPU type is Unknown.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                    Engine::Log::Info("GPU type is Integrated.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                    Engine::Log::Info("GPU type is Discrete.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                    Engine::Log::Info("GPU type is Virtual.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_CPU: Engine::Log::Info("GPU type is CPU."); break;
            }
            Engine::Log::Info("GPU Driver version: %d.%d.%d",
                              VK_VERSION_MAJOR(properties.driverVersion),
                              VK_VERSION_MINOR(properties.driverVersion),
                              VK_VERSION_PATCH(properties.driverVersion));
            Engine::Log::Info(
              "Vulkan API version: %d.%d.%d", VK_VERSION_MAJOR(properties.apiVersion),
              VK_VERSION_MINOR(properties.apiVersion), VK_VERSION_PATCH(properties.apiVersion));
            for ( u32 j = 0; j < memory.memoryHeapCount; ++j ) {
                f32 memory_size_gib =
                  (((f32)memory.memoryHeaps[j].size) / 1024.0f / 1024.0f / 1024.0f);
                if ( memory.memoryHeaps[j].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT ) {
                    Engine::Log::Info("Local GPU memory: %.2f GiB", memory_size_gib);
                } else {
                    Engine::Log::Info("Shared System memory: %.2f GiB", memory_size_gib);
                }
            }
            physical    = physical_devices[i];
            mProperties = properties;
            mFeatures   = features;
            mMemory     = memory;
            break;
        }
    }
    if ( !physical ) {
        Engine::Log::Error("No physical devices were found which met the requirements.");
        return false;
    }
    Engine::Log::Info("Physical device selected.");
    return true;
}

b8 Device::physical_device_meets_requirements(
  VkPhysicalDevice device, VkSurfaceKHR surface, VkPhysicalDeviceProperties properties,
  VkPhysicalDeviceFeatures features, [[maybe_unused]] VkPhysicalDeviceMemoryProperties memory) {
    QueueFamilyInfo queue_info = {};

    if ( mRequirements.discrete_gpu ) {
        if ( properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ) {
            Engine::Log::Info("Device is not a discrete GPU, and one is required. Skipping.");
            mQueueFamily = queue_info;
            return false;
        }
    }
    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
    Engine::DArray<VkQueueFamilyProperties> queue_families =
      Engine::DArray<VkQueueFamilyProperties>(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());
    Engine::Log::Info("Graphics | Present | Compute | Transfer | Name");
    u8 min_transfer_score = 255;
    for ( u32 i = 0; i < queue_family_count; ++i ) {
        u8 current_transfer_score = 0;
        if ( queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT ) {
            queue_info.graphics_index = i;
            ++current_transfer_score;
        }
        if ( queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT ) {
            queue_info.compute_index = i;
            ++current_transfer_score;
        }
        if ( queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT ) {
            if ( current_transfer_score <= min_transfer_score ) {
                min_transfer_score        = current_transfer_score;
                queue_info.transfer_index = i;
            }
        }
        VkBool32 supports_present = VK_FALSE;
        VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &supports_present));
        if ( supports_present ) { queue_info.present_index = i; }
    }
    Engine::Log::Info("       %d |       %d |       %d |        %d | %s",
                      queue_info.graphics_index != -1, queue_info.present_index != -1,
                      queue_info.compute_index != -1, queue_info.transfer_index != -1,
                      properties.deviceName);

    if ( (!mRequirements.graphics || (mRequirements.graphics && queue_info.graphics_index != -1)) &&
         (!mRequirements.present || (mRequirements.present && queue_info.present_index != -1)) &&
         (!mRequirements.compute || (mRequirements.compute && queue_info.compute_index != -1)) &&
         (!mRequirements.transfer || (mRequirements.transfer && queue_info.transfer_index != 1)) ) {
        Engine::Log::Info("Device meets queue requirements.");
        Engine::Log::Trace("Graphics Family Index: %i", queue_info.graphics_index);
        Engine::Log::Trace("Present Family Index: %i", queue_info.present_index);
        Engine::Log::Trace("Transfer Family Index: %i", queue_info.transfer_index);
        Engine::Log::Trace("Compute Family Index: %i", queue_info.compute_index);
        mQueueFamily = queue_info;
        Device::QuerySwapchainSupport(device, surface);
        const auto& support = Device::Get().GetSwapchainSupport();
        if ( support.format_count < 1 || support.present_mode_count < 1 ) {
            if ( support.formats ) {
                Engine::Memory::df_free(support.formats,
                                        sizeof(VkSurfaceFormatKHR) * support.format_count,
                                        Engine::Memory::Tag::RENDERER);
            }
            if ( support.present_modes ) {
                Engine::Memory::df_free(support.present_modes,
                                        sizeof(VkPresentModeKHR) * support.present_mode_count,
                                        Engine::Memory::Tag::RENDERER);
            }
            Engine::Log::Info("Required swapchain support not present, skipping device.");
            return false;
        }
        if ( !mRequirements.device_extension_names.isEmpty() ) {
            u32 available_extension_count               = 0;
            VkExtensionProperties* available_extensions = nullptr;
            VK_CHECK(vkEnumerateDeviceExtensionProperties(device, nullptr,
                                                          &available_extension_count, nullptr));
            if ( available_extension_count != 0 ) {
                available_extensions =
                  static_cast<VkExtensionProperties*>(Engine::Memory::df_allocate(
                    sizeof(VkExtensionProperties) * available_extension_count,
                    Engine::Memory::Tag::RENDERER));
                VK_CHECK(vkEnumerateDeviceExtensionProperties(
                  device, nullptr, &available_extension_count, available_extensions));
                u32 required_extension_count =
                  static_cast<u64>(mRequirements.device_extension_names.size());
                for ( u32 i = 0; i < required_extension_count; ++i ) {
                    b8 found = false;
                    for ( u32 j = 0; j < available_extension_count; ++j ) {
                        if ( Engine::strs_eq(mRequirements.device_extension_names[i],
                                             available_extensions[j].extensionName) ) {
                            found = true;
                            break;
                        }
                    }
                    if ( !found ) {
                        Engine::Log::Info("Required extension not found: '%s', skipping device.",
                                          mRequirements.device_extension_names[i]);
                        Engine::Memory::df_free(available_extensions,
                                                sizeof(VkExtensionProperties) *
                                                  available_extension_count,
                                                Engine::Memory::Tag::RENDERER);
                        return false;
                    }
                }
            }
            Engine::Memory::df_free(available_extensions,
                                    sizeof(VkExtensionProperties) * available_extension_count,
                                    Engine::Memory::Tag::RENDERER);
        }
        if ( mRequirements.sampler_anisotropy && !features.samplerAnisotropy ) {
            Engine::Log::Info("Device does not support samplerAnisotropy, skipping.");
            return false;
        }
        // Device meets all requirements
        return true;
    }
    mQueueFamily = queue_info;
    return false;
}
}  // namespace Vulkan
