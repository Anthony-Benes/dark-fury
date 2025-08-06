#include "vulkan_backend.hpp"
#include <containers/darray.hpp>
#include <core/df_string.hpp>
#include <core/logger.hpp>
#include <platform/platform.hpp>
#include <renderer/vulkan/vulkan_device.hpp>
#include <renderer/vulkan/vulkan_platform.hpp>
#include <renderer/vulkan/vulkan_types.inl>

#if defined(_DEBUG)
VKAPI_ATTR VkBool32 VKAPI_CALL
  vk_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                    VkDebugUtilsMessageTypeFlagsEXT message_types,
                    const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data);
#endif

namespace Renderer {
static vulkan_context context;

Backend* CreateBackend(const char* application_name, platform_state* plat_state) {
    return new VulkanBackend(application_name, plat_state);
}

VulkanBackend::VulkanBackend(const char* application_name, platform_state* plat_state)
  : Backend(application_name, plat_state) {
    Initialize(application_name, plat_state);
}

b8 VulkanBackend::Initialize(const char* application_name,
                             [[maybe_unused]] platform_state* plat_state) {
    context.allocator = 0;

    u32 instance_version = VK_API_VERSION_1_0;
    vkEnumerateInstanceVersion(&instance_version);
    VkApplicationInfo app_info  = {};
    app_info.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.apiVersion         = instance_version;
    app_info.pApplicationName   = application_name;
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName        = "Dark Fury Engine";
    app_info.engineVersion      = VK_MAKE_VERSION(1, 0, 0);

    VkInstanceCreateInfo create_info = {};
    create_info.sType                = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo     = &app_info;
    Engine::DArray<const char*> required_extensions;
    required_extensions.push(VK_KHR_SURFACE_EXTENSION_NAME);
    platform_get_required_extension_names(&required_extensions);
#if defined(_DEBUG)
    required_extensions.push(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    Engine::Log::Debug("Required extensions:");
    for ( u32 i = 0; i < required_extensions.size(); i++ ) {
        Engine::Log::Debug(required_extensions[i]);
    }
#endif
    create_info.enabledExtensionCount   = static_cast<u32>(required_extensions.size());
    create_info.ppEnabledExtensionNames = required_extensions.data();
    Engine::DArray<const char*> required_validation_layers;
    u32 required_validation_layer_count = 0;
#if defined(_DEBUG)
    Engine::Log::Info("Validation layers enabled. Enumerating...");
    required_validation_layers.push("VK_LAYER_KHRONOS_validation");
    required_validation_layer_count = static_cast<u32>(required_validation_layers.size());
    u32 available_layer_count       = 0;
    VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layer_count, 0));
    Engine::DArray<VkLayerProperties> available_layers(available_layer_count);
    VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layer_count, available_layers.data()));
    for ( u32 i = 0; i < required_validation_layer_count; i++ ) {
        Engine::Log::Info("Searching for layer: %s...", required_validation_layers[i]);
        b8 found = false;
        for ( u32 j = 0; j < available_layer_count; j++ ) {
            if ( Engine::strs_eq(required_validation_layers[i], available_layers[j].layerName) ) {
                found = true;
                Engine::Log::Info("Found.");
                break;
            }
        }
        if ( !found ) {
            Engine::Log::Fatal("Required validation layer is missing: %s",
                               required_validation_layers[i]);
            return false;
        }
    }
    Engine::Log::Info("All required validation layers are present.");
#endif
    create_info.enabledLayerCount   = required_validation_layer_count;
    create_info.ppEnabledLayerNames = required_validation_layers.data();

    VK_CHECK(vkCreateInstance(&create_info, context.allocator, &context.instance));
    Engine::Log::Info("Vulkan instance created.");
#if defined(_DEBUG)
    Engine::Log::Debug("Creating Vulkan debugger...");
    u32 log_severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
    VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {};
    debug_create_info.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_create_info.messageSeverity = log_severity;
    debug_create_info.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    debug_create_info.pfnUserCallback = vk_debug_callback;
    debug_create_info.pUserData       = 0;
    PFN_vkCreateDebugUtilsMessengerEXT func =
      (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context.instance,
                                                                "vkCreateDebugUtilsMessengerEXT");
    DF_ASSERT_MSG(func, "Failed to create debug messenger!");
    VK_CHECK(
      func(context.instance, &debug_create_info, context.allocator, &context.debug_messenger));
    Engine::Log::Debug("Vulkan debugger created.");
#endif

    Engine::Log::Debug("Creating Vulkan surface...");
    if ( !platform_create_vulkan_surface(plat_state, &context) ) {
        Engine::Log::Error("Failed to create platform surface!");
        return false;
    }
    Engine::Log::Debug("Vulkan surface created.");

    if ( !Vulkan::Device::Create(&context) ) {
        Engine::Log::Error("Failed to create device!");
        return false;
    }

    Engine::Log::Info("Vulkan renderer initialized successfully.");
    return true;
}

void VulkanBackend::Shutdown() {
    Engine::Log::Debug("Destroying Vulkan device...");
    Vulkan::Device::Destroy(&context);
    Engine::Log::Debug("Destroying Vulkan surface...");
    if ( context.surface ) {
        vkDestroySurfaceKHR(context.instance, context.surface, context.allocator);
        context.surface = 0;
    }
#if defined(_DEBUG)
    Engine::Log::Debug("Destroying Vulkan debugger...");
    if ( context.debug_messenger ) {
        PFN_vkDestroyDebugUtilsMessengerEXT func =
          (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            context.instance, "vkDestroyDebugUtilsMessengerEXT");
        func(context.instance, context.debug_messenger, context.allocator);
    }
#endif
    Engine::Log::Debug("Destroying Vulkan instance...");
    vkDestroyInstance(context.instance, context.allocator);
}

void VulkanBackend::OnResized([[maybe_unused]] u16 width, [[maybe_unused]] u16 height) {}

}  // namespace Renderer

#if defined(_DEBUG)
VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
  VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
  [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT message_types,
  const VkDebugUtilsMessengerCallbackDataEXT* callback_data, [[maybe_unused]] void* user_data) {
    switch ( message_severity ) {
        default:
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            Engine::Log::Error(callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            Engine::Log::Warn(callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            Engine::Log::Info(callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            Engine::Log::Trace(callback_data->pMessage);
            break;
    }
    return VK_FALSE;
}
#endif
