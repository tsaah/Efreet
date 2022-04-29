#include "Device.h"

#include <vector>

namespace efreet::engine::renderer::vulkan::device {

namespace {

struct QueueFamilyInfo {
    u32 graphicsFamilyIndex{ static_cast<u32>(-1) };
    u32 presentFamilyIndex{ static_cast<u32>(-1) };
    u32 computeFamilyIndex{ static_cast<u32>(-1) };
    u32 transferFamilyIndex{ static_cast<u32>(-1) };
};

inline void querySwapchainSupport(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface, SwapchainSupportInfo& swapchainSupportinfo) {
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &swapchainSupportinfo.capabilities));

    u32 formatCount = 0;
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr));
    if (formatCount != 0) {
        swapchainSupportinfo.formats.resize(formatCount);
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, swapchainSupportinfo.formats.data()));
    }

    u32 presentModeCount = 0;
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr));
    if (presentModeCount != 0) {
        swapchainSupportinfo.presentModes.resize(presentModeCount);
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, swapchainSupportinfo.presentModes.data()));
    }
}

inline b8 physicalDeviceMeetsRequirements(
    const VkPhysicalDevice& device,
    const VkSurfaceKHR& surface,
    const VkPhysicalDeviceProperties& properties,
    const VkPhysicalDeviceFeatures& features,
    const Requirements& requirements,
    QueueFamilyInfo& queueInfo,
    SwapchainSupportInfo& swapchainSupportInfo) {
    // Evaluate device properties to determine if it meets the needs of our applcation.
    queueInfo.graphicsFamilyIndex = static_cast<u32>(-1);
    queueInfo.presentFamilyIndex = static_cast<u32>(-1);
    queueInfo.computeFamilyIndex = static_cast<u32>(-1);
    queueInfo.transferFamilyIndex = static_cast<u32>(-1);

    // Discrete GPU?
    if (requirements.discreteGpu) {
        if (properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            E_INFO("Device is not a discrete GPU, and one is required. Skipping.");
            return false;
        }
    }

    u32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    // Look at each queue and see what queues it supports
    E_INFO("Graphics | Present | Compute | Transfer | Name");
    u8 minTransferScore = 255;
    for (u32 i = 0; i < queueFamilyCount; ++i) {
        u8 currentTransferScore = 0;

        // Graphics queue?
        if (queueInfo.graphicsFamilyIndex == -1 && queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            queueInfo.graphicsFamilyIndex = i;
            ++currentTransferScore;

            // If also a present queue, this prioritizes grouping of the 2.
            VkBool32 supportsPresent{ VK_FALSE };
            VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &supportsPresent));
            if (supportsPresent) {
                queueInfo.presentFamilyIndex = i;
                ++currentTransferScore;
            }
        }

        // Compute queue?
        if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            queueInfo.computeFamilyIndex = i;
            ++currentTransferScore;
        }

        // Transfer queue?
        if (queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            // Take the index if it is the current lowest. This increases the
            // liklihood that it is a dedicated transfer queue.
            if (currentTransferScore <= minTransferScore) {
                minTransferScore = currentTransferScore;
                queueInfo.transferFamilyIndex = i;
            }
        }
    }

    // If a present queue hasn't been found, iterate again and take the first one.
    // This should only happen if there is a queue that supports graphics but NOT
    // present.
    if (queueInfo.presentFamilyIndex == -1) {
        for (u32 i = 0; i < queueFamilyCount; ++i) {
            VkBool32 supportsPresent{ VK_FALSE };
            VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &supportsPresent));
            if (supportsPresent) {
                queueInfo.presentFamilyIndex = i;

                // If they differ, bleat about it and move on. This is just here for troubleshooting
                // purposes.
                if (queueInfo.presentFamilyIndex != queueInfo.graphicsFamilyIndex) {
                    E_WARN("Warning: Different queue index used for present vs graphics: %u.", i);
                }
                break;
            }
        }
    }

    // Print out some info about the device
    E_INFO("       %d |       %d |       %d |        %d | %s",
          queueInfo.graphicsFamilyIndex != -1,
          queueInfo.presentFamilyIndex != -1,
          queueInfo.computeFamilyIndex != -1,
          queueInfo.transferFamilyIndex != -1,
          properties.deviceName);

    if (
        (!requirements.graphics || (requirements.graphics && queueInfo.graphicsFamilyIndex != -1)) &&
        (!requirements.present || (requirements.present && queueInfo.presentFamilyIndex != -1)) &&
        (!requirements.compute || (requirements.compute && queueInfo.computeFamilyIndex != -1)) &&
        (!requirements.transfer || (requirements.transfer && queueInfo.transferFamilyIndex != -1))) {
        E_INFO("Device meets queue requirements.");
        E_TRACE("Graphics Family Index: %i", queueInfo.graphicsFamilyIndex);
        E_TRACE("Present Family Index:  %i", queueInfo.presentFamilyIndex);
        E_TRACE("Transfer Family Index: %i", queueInfo.transferFamilyIndex);
        E_TRACE("Compute Family Index:  %i", queueInfo.computeFamilyIndex);

        // Query swapchain support.
        querySwapchainSupport(
            device,
            surface,
            swapchainSupportInfo);

        if (swapchainSupportInfo.formats.size() < 1 || swapchainSupportInfo.presentModes.size() < 1) {
            swapchainSupportInfo.formats.clear();
            swapchainSupportInfo.presentModes.clear();
            E_INFO("Required swapchain support not present, skipping device.");
            return false;
        }

        // Device extensions.
        if (!requirements.deviceExtensionNames.empty()) {
            u32 availableExtensionCount = 0;
            VK_CHECK(vkEnumerateDeviceExtensionProperties(device, nullptr, &availableExtensionCount, nullptr));
            if (availableExtensionCount != 0) {
                std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
                VK_CHECK(vkEnumerateDeviceExtensionProperties(device, nullptr, &availableExtensionCount, availableExtensions.data()));

                const auto requiredExtensionCount = requirements.deviceExtensionNames.size();
                for (u32 i = 0; i < requiredExtensionCount; ++i) {
                    b8 found{ false };
                    for (u32 j = 0; j < availableExtensionCount; ++j) {
                        if (::strcmp(requirements.deviceExtensionNames[i], availableExtensions[j].extensionName) == 0) { // TODO: strcmp
                            found = true;
                            break;
                        }
                    }

                    if (!found) {
                        E_INFO("Required extension not found: '%s', skipping device.", requirements.deviceExtensionNames[i]);
                        availableExtensions.clear();
                        return false;
                    }
                }
            }
        }

        // Sampler anisotropy
        if (requirements.samplerAnisotropy && !features.samplerAnisotropy) {
            E_INFO("Device does not support samplerAnisotropy, skipping.");
            return false;
        }

        // Device meets all requirements.
        return true;
    }
    return false;
}

inline b8 selectPhysicalDevice(Context& context, const Requirements& requirements) {
    u32 physicalDeviceCount = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(context.instance, &physicalDeviceCount, nullptr));
    if (physicalDeviceCount == 0) {
        E_FATAL("No devices which support Vulkan were found.");
        return false;
    }

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    VK_CHECK(vkEnumeratePhysicalDevices(context.instance, &physicalDeviceCount, physicalDevices.data()));
    for (const auto& physicalDevice: physicalDevices) {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physicalDevice, &properties);

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(physicalDevice, &features);

        VkPhysicalDeviceMemoryProperties memory;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memory);

        E_INFO("Evaluating device: '%s'", properties.deviceName);

        // Check if device supports local/host visible combo
        b8 supportsDeviceLocalHostVisible{ false };
        for (u32 i = 0; i < memory.memoryTypeCount; ++i) {
            // Check each memory type to see if its bit is set to 1.
            if (
                ((memory.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0) &&
                ((memory.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != 0)) {
                supportsDeviceLocalHostVisible = true;
                break;
            }
        }

        QueueFamilyInfo queueInfo = {};
        b8 result = physicalDeviceMeetsRequirements(
            physicalDevice,
            context.surface,
            properties,
            features,
            requirements,
            queueInfo,
            context.device.swapchainSupportInfo);

        if (result) {
            E_INFO("Selected device: '%s'.", properties.deviceName);
            // GPU type, etc.
            switch (properties.deviceType) {
                default:
                case VK_PHYSICAL_DEVICE_TYPE_OTHER:
                    E_INFO("GPU type is Unknown.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                    E_INFO("GPU type is Integrated.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                    E_INFO("GPU type is Descrete.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                    E_INFO("GPU type is Virtual.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_CPU:
                    E_INFO("GPU type is CPU.");
                    break;
            }

            E_INFO(
                "GPU Driver version: %d.%d.%d",
                VK_VERSION_MAJOR(properties.driverVersion),
                VK_VERSION_MINOR(properties.driverVersion),
                VK_VERSION_PATCH(properties.driverVersion));

            // Vulkan API version.
            E_INFO(
                "Vulkan API version: %d.%d.%d",
                VK_VERSION_MAJOR(properties.apiVersion),
                VK_VERSION_MINOR(properties.apiVersion),
                VK_VERSION_PATCH(properties.apiVersion));

            // Memory information
            for (u32 j = 0; j < memory.memoryHeapCount; ++j) {
                const auto memorySizeGib = static_cast<f32>(memory.memoryHeaps[j].size) / 1024.0f / 1024.0f / 1024.0f;
                if (memory.memoryHeaps[j].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
                    E_INFO("Local GPU memory: %.2f GiB", memorySizeGib);
                } else {
                    E_INFO("Shared System memory: %.2f GiB", memorySizeGib);
                }
            }

            context.device.physicalDevice = physicalDevice;
            context.device.graphicsQueueIndex = queueInfo.graphicsFamilyIndex;
            context.device.presentQueueIndex = queueInfo.presentFamilyIndex;
            context.device.computeQueueIndex = queueInfo.computeFamilyIndex;
            context.device.transferQueueIndex = queueInfo.transferFamilyIndex;

            // Keep a copy of properties, features and memory info for later use.
            context.device.properties = properties;
            context.device.features = features;
            context.device.memory = memory;
            context.device.supportsDeviceLocalHostVisible = supportsDeviceLocalHostVisible;
            break;
        }
    }

    // Ensure a device was selected
    if (context.device.physicalDevice == nullptr) {
        E_ERROR("No physical devices were found which meet the requirements.");
        return false;
    }

    E_INFO("Physical device selected.");
    return true;
}


} // namespace

b8 create(Context& context, const Requirements& requirements) {
    if (!selectPhysicalDevice(context, requirements)) {
        return false;
    }

    E_INFO("Creating logical device...");
    // NOTE: Do not create additional queues for shared indices.
    std::vector<u32> indices;
    indices.push_back(context.device.graphicsQueueIndex);
    if (context.device.graphicsQueueIndex != context.device.presentQueueIndex) {
        indices.push_back(context.device.presentQueueIndex);
    }
    if (context.device.graphicsQueueIndex != context.device.transferQueueIndex) {
        indices.push_back(context.device.transferQueueIndex);
    }
    if (context.device.graphicsQueueIndex != context.device.computeQueueIndex) {
        indices.push_back(context.device.computeQueueIndex);
    }

    const auto indexCount = indices.size();
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos(indexCount);
    for (u32 i = 0; i < indexCount; ++i) {
        queueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfos[i].queueFamilyIndex = indices[i];
        queueCreateInfos[i].queueCount = 1;
        queueCreateInfos[i].flags = 0;
        queueCreateInfos[i].pNext = 0;
        f32 queuePriority = 1.0f;
        queueCreateInfos[i].pQueuePriorities = &queuePriority;
    }

    // Request device features.
    // TODO: should be config driven
    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;  // Request anistrophy

    b8 portabilityRequired = false;
    u32 availableExtensionCount = 0;
    VK_CHECK(vkEnumerateDeviceExtensionProperties(context.device.physicalDevice, nullptr, &availableExtensionCount, nullptr));
    if (availableExtensionCount != 0) {
        std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
        VK_CHECK(vkEnumerateDeviceExtensionProperties(context.device.physicalDevice, nullptr, &availableExtensionCount, availableExtensions.data()));
        for (u32 i = 0; i < availableExtensionCount; ++i) {
            if (::strcmp(availableExtensions[i].extensionName, "VK_KHR_portability_subset") == 0) { //TODO: strcmp
                E_INFO("Adding required extension 'VK_KHR_portability_subset'.");
                portabilityRequired = true;
                break;
            }
        }
    }

    std::vector<const char*> extensionNames = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    if (portabilityRequired) {
        extensionNames.push_back("VK_KHR_portability_subset");
    }

    VkDeviceCreateInfo deviceCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
    deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size();
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    deviceCreateInfo.enabledExtensionCount = extensionNames.size();
    deviceCreateInfo.ppEnabledExtensionNames = extensionNames.data();

    // Create the device.
    VK_CHECK(vkCreateDevice(context.device.physicalDevice, &deviceCreateInfo, context.allocator, &context.device.logicalDevice));

    E_INFO("Logical device created.");

    // Get queues.
    vkGetDeviceQueue(context.device.logicalDevice, context.device.graphicsQueueIndex, 0, &context.device.graphicsQueue);
    vkGetDeviceQueue(context.device.logicalDevice, context.device.presentQueueIndex, 0, &context.device.presentQueue);
    vkGetDeviceQueue(context.device.logicalDevice, context.device.computeQueueIndex, 0, &context.device.computeQueue);
    vkGetDeviceQueue(context.device.logicalDevice, context.device.transferQueueIndex, 0, &context.device.transferQueue);
    E_INFO("Queues obtained.");

    // Create command pool for graphics queue.
    VkCommandPoolCreateInfo poolCreateInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
    poolCreateInfo.queueFamilyIndex = context.device.graphicsQueueIndex;
    poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    VK_CHECK(vkCreateCommandPool(context.device.logicalDevice, &poolCreateInfo, context.allocator, &context.device.graphicsCommandPool));
    E_INFO("Graphics command pool created.");

    return true;
}

void destroy(Context& context) {
    // Unset queues
    context.device.graphicsQueue = nullptr;
    context.device.presentQueue = nullptr;
    context.device.computeQueue = nullptr;
    context.device.transferQueue = nullptr;

    E_INFO("Destroying command pools...");
    vkDestroyCommandPool(context.device.logicalDevice, context.device.graphicsCommandPool, context.allocator);

    // Destroy logical device
    E_INFO("Destroying logical device...");
    if (context.device.logicalDevice) {
        vkDestroyDevice(context.device.logicalDevice, context.allocator);
        context.device.logicalDevice = nullptr;
    }

    // Physical devices are not destroyed.
    E_INFO("Releasing physical device resources...");
    context.device.physicalDevice = nullptr;

    context.device.swapchainSupportInfo.formats.clear();
    context.device.swapchainSupportInfo.presentModes.clear();

    context.device.swapchainSupportInfo.capabilities = {};

    context.device.graphicsQueueIndex = -1;
    context.device.presentQueueIndex = -1;
    context.device.computeQueueIndex = -1;
    context.device.transferQueueIndex = -1;
}

} // namespace efreet::engine::renderer::vulkan::device