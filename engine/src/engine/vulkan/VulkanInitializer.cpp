#include "engine/vulkan/VulkanInitializer.h"

#include "engine/renderer/RendererUtils.h"
#include "engine/system/System.h"
#include "engine/utils/Logger.h"
#include "engine/vulkan/IVulkanContext.h"

#include <SDL3/SDL_vulkan.h>
#include <string>
#include <vulkan/vulkan.h>

namespace {
VkInstance CreateVulkanInstance(std::string appName) {
    uint32_t extensionCount = 0;
    if (!SDL_Vulkan_GetInstanceExtensions(&extensionCount)) {
        LOG_FATAL("Failed to get Vulkan extension count: {}", SDL_GetError());
        return VK_NULL_HANDLE;
    }

    auto extensions = SDL_Vulkan_GetInstanceExtensions(&extensionCount);
    if (!extensions) {
        LOG_FATAL("Failed to get Vulkan extensions: {}", SDL_GetError());
        return VK_NULL_HANDLE;
    }
    std::vector<const char*> extensionsVec(extensions,
                                           extensions + extensionCount);

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = appName.c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "TiredEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

#ifdef DEBUG_BUILD
    // Check if validation layers are available
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    bool validationLayerFound = false;
    for (const auto& layerProperties : availableLayers) {
        if (strcmp(layerProperties.layerName, "VK_LAYER_KHRONOS_validation") ==
            0) {
            validationLayerFound = true;
            break;
        }
    }

    if (validationLayerFound) {
        const char* validationLayers[] = {"VK_LAYER_KHRONOS_validation"};
        createInfo.enabledLayerCount = 1;
        createInfo.ppEnabledLayerNames = validationLayers;
        LOG_INFO("Vulkan validation layers enabled");

        // Check if debug utils extension is available
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                               nullptr);
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                               availableExtensions.data());

        bool hasDebugUtils = false;
        for (const auto& ext : availableExtensions) {
            if (strcmp(ext.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) ==
                0) {
                hasDebugUtils = true;
                break;
            }
        }

        if (hasDebugUtils) {
            extensionsVec.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            LOG_INFO("Vulkan debug utils extension enabled");
        } else {
            LOG_WARNING("Debug utils extension not available on this platform");
        }
    } else {
        createInfo.enabledLayerCount = 0;
        LOG_WARNING("VK_LAYER_KHRONOS_validation not found - validation "
                    "disabled. Install Vulkan SDK to enable validation.");
    }
#else
    createInfo.enabledLayerCount = 0;
#endif
#ifdef __APPLE__
    extensionsVec.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif
    extensionsVec.push_back(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);

    createInfo.enabledExtensionCount =
        static_cast<uint32_t>(extensionsVec.size());
    createInfo.ppEnabledExtensionNames = extensionsVec.data();
    VkInstance instance;

    if (auto result = vkCreateInstance(&createInfo, nullptr, &instance);
        result != VK_SUCCESS) {
        LOG_FATAL("Couldn't create Vulkan instance! Error code: {}",
                  static_cast<int>(result));
        return VK_NULL_HANDLE;
    }
    return instance;
}

VkDebugUtilsMessengerEXT CreateDebugUtilsMessenger(VkInstance instance) {
#ifdef DEBUG_BUILD
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = &VulkanDebugCallback;
    createInfo.pUserData = nullptr;

    auto createDebugCallback =
        (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            instance, "vkCreateDebugUtilsMessengerEXT");

    if (createDebugCallback == nullptr) {
        LOG_ERROR("Failed to set up debug callback!");
        return VK_NULL_HANDLE;
    }

    VkDebugUtilsMessengerEXT debugMessenger;
    if (createDebugCallback(instance, &createInfo, nullptr, &debugMessenger) !=
        VK_SUCCESS) {
        LOG_ERROR("Failed to set up debug callback!");
        return VK_NULL_HANDLE;
    }

    return debugMessenger;
#else
    return VK_NULL_HANDLE;
#endif
}

VkSurfaceKHR CreateVulkanSurface(SDL_Window* window, VkInstance instance) {
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    if (!SDL_Vulkan_CreateSurface(window, instance, nullptr, &surface)) {
        LOG_ERROR("Failed to create Vulkan surface: {}", SDL_GetError());
        return VK_NULL_HANDLE;
    }
    return surface;
}

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() const {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device,
                                     VkSurfaceKHR surface) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                             nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                             queueFamilies.data());

    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        // Graphics queue
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            indices.graphicsFamily = i;

        // Presentation support
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface,
                                             &presentSupport);
        if (presentSupport)
            indices.presentFamily = i;

        if (indices.isComplete())
            break;
    }

    return indices;
}

std::tuple<VkPhysicalDevice, VkPhysicalDeviceProperties,
           VkPhysicalDeviceFeatures, VkPhysicalDeviceMemoryProperties, uint32_t,
           uint32_t>
PickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        LOG_FATAL("No Vulkan-capable GPU found!");
        return {};
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    for (const auto& device : devices) {
        QueueFamilyIndices indices = FindQueueFamilies(device, surface);
        if (indices.isComplete()) {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(device, &properties);

            VkPhysicalDeviceFeatures features;
            vkGetPhysicalDeviceFeatures(device, &features);

            VkPhysicalDeviceMemoryProperties memProperties;
            vkGetPhysicalDeviceMemoryProperties(device, &memProperties);

            return {device,
                    properties,
                    features,
                    memProperties,
                    indices.graphicsFamily.value(),
                    indices.presentFamily.value()};
        }
    }

    LOG_FATAL("Failed to find a suitable GPU!");
    return {};
}

std::tuple<VkDevice, VkQueue, VkQueue>
CreateLogicalDevice(uint32_t graphicsFamily, uint32_t presentFamily,
                    VkPhysicalDevice physicalDevice) {

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    auto uniqueQueueFamilies = {graphicsFamily, presentFamily};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    // Check for available device extensions
    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr,
                                         &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(
        physicalDevice, nullptr, &extensionCount, availableExtensions.data());

    std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#ifdef __APPLE__
    // Check if portability subset is available (required on MoltenVK)
    bool hasPortabilitySubset = false;
    for (const auto& ext : availableExtensions) {
        if (strcmp(ext.extensionName, "VK_KHR_portability_subset") == 0) {
            hasPortabilitySubset = true;
            break;
        }
    }

    if (hasPortabilitySubset) {
        deviceExtensions.push_back("VK_KHR_portability_subset");
        LOG_INFO("Enabling VK_KHR_portability_subset extension");
    }
#endif

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount =
        static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount =
        static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    VkDevice device;
    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) !=
        VK_SUCCESS) {
        LOG_FATAL("Failed to create logical device!");
        return {};
    }

    VkQueue graphicsQueue, presentQueue;
    vkGetDeviceQueue(device, graphicsFamily, 0, &graphicsQueue);
    vkGetDeviceQueue(device, presentFamily, 0, &presentQueue);

    return {device, graphicsQueue, presentQueue};
}

std::tuple<VkSurfaceCapabilitiesKHR, std::vector<VkSurfaceFormatKHR>,
           std::vector<VkPresentModeKHR>>
QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
                                         nullptr);
    formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
                                         formats.data());

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface,
                                              &presentModeCount, nullptr);
    presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, surface, &presentModeCount, presentModes.data());

    return {};
}

VkSurfaceFormatKHR
ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats) {
    for (const auto& f : formats) {
        if (f.format == VK_FORMAT_B8G8R8A8_SRGB &&
            f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return f;
    }
    return formats[0];
}

VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& modes) {
    for (const auto& m : modes)
        if (m == VK_PRESENT_MODE_MAILBOX_KHR)
            return m;
    return VK_PRESENT_MODE_FIFO_KHR; // guaranteed
}

VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& caps,
                        SDL_Window* window) {
    if (caps.currentExtent.width != UINT32_MAX)
        return caps.currentExtent;

    int w, h;
    SDL_GetWindowSizeInPixels(window, &w, &h);
    VkExtent2D actualExtent = {static_cast<uint32_t>(w),
                               static_cast<uint32_t>(h)};
    actualExtent.width =
        std::clamp(actualExtent.width, caps.minImageExtent.width,
                   caps.maxImageExtent.width);
    actualExtent.height =
        std::clamp(actualExtent.height, caps.minImageExtent.height,
                   caps.maxImageExtent.height);
    return actualExtent;
}

std::tuple<VkSwapchainKHR, VkFormat, VkExtent2D, std::vector<VkImage>,
           std::vector<VkImageView>>
CreateSwapchain(VkPhysicalDevice physicalDevice, VkDevice device,
                VkSurfaceKHR surface, SDL_Window* window,
                uint32_t graphicsFamily, uint32_t presentFamily) {
    auto [capabilities, formats, presentModes] =
        QuerySwapChainSupport(physicalDevice, surface);
    VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(formats);
    VkPresentModeKHR presentMode = ChoosePresentMode(presentModes);
    VkExtent2D extent = ChooseExtent(capabilities, window);
    uint32_t imageCount = capabilities.minImageCount + 1;

    if (capabilities.maxImageCount > 0 &&
        imageCount > capabilities.maxImageCount)
        imageCount = capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage =
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    uint32_t queueFamilyIndices[] = {graphicsFamily, presentFamily};
    if (graphicsFamily != presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkFormat imageFormat;
    VkExtent2D extent;
    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
    if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchain) !=
        VK_SUCCESS) {
        LOG_FATAL("Failed to create swapchain!");
    }

    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
    images.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, images.data());
    imageFormat = surfaceFormat.format;
    extent = extent;

    // Create image views
    imageViews.resize(images.size());
    for (size_t i = 0; i < images.size(); i++) {
        Renderer::CreateImageView(device, images[i], imageFormat,
                                  VK_IMAGE_ASPECT_COLOR_BIT, imageViews[i]);
    }

    return {swapchain, imageFormat, extent, images, imageViews};
}

VkRenderPass CreateRenderPass(VkDevice device, VkFormat swapchainFormat) {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapchainFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    VkRenderPass renderPass;
    if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) !=
        VK_SUCCESS) {
        LOG_FATAL("Failed to create render pass!");
    }
    return renderPass;
}

VkCommandPool CreateCommandBufferPool(VkDevice device,
                                      uint32_t queueFamilyIndex) {
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndex;

    VkCommandPool commandPool;
    if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) !=
        VK_SUCCESS) {
        LOG_FATAL("Failed to create command pool!");
    }
    return commandPool;
}

std::vector<VkCommandBuffer> CreateCommandBuffers(VkDevice device,
                                                  VkCommandPool& commandPool,
                                                  int bufferCount) {
    std::vector<VkCommandBuffer> commandBuffers;
    commandBuffers.resize(bufferCount);
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(bufferCount);

    if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) !=
        VK_SUCCESS) {
        LOG_FATAL("Failed to allocate command buffers!");
    }
    return commandBuffers;
}

std::vector<VkFramebuffer>
CreateFramebuffers(VkDevice device, VkRenderPass renderPass,
                   const std::vector<VkImageView>& imageViews,
                   VkExtent2D extent) {
    std::vector<VkFramebuffer> framebuffers(imageViews.size());
    for (size_t i = 0; i < imageViews.size(); i++) {
        VkImageView attachments[] = {imageViews[i]};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = extent.width; // TODO: Might be an issue here.
        framebufferInfo.height = extent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr,
                                &framebuffers[i]) != VK_SUCCESS) {
            LOG_FATAL("Failed to create framebuffer!");
        }
    }
    return framebuffers;
}

} // namespace

namespace Vulkan {
void InitializeVulkan(IVulkanContextSetter& context, SDL_Window* window,
                      const System::CSystem& system) {
    VkInstance instance = CreateVulkanInstance(system.GetGameName());

    context.SetInstance(instance);
    context.SetDebugMessenger(CreateDebugUtilsMessenger(instance));

    VkSurfaceKHR surface = CreateVulkanSurface(window, instance);
    context.SetSurface(surface);

    auto [physicalDevice, properties, features, memoryProperties,
          graphicsFamily, presentFamily] =
        PickPhysicalDevice(instance, surface);
    context.SetPhysicalDevice(physicalDevice);
    context.SetPhysicalDeviceProperties(properties);
    context.SetPhysicalDeviceFeatures(features);
    context.SetPhysicalDeviceMemoryProperties(memoryProperties);
    context.SetGraphicsQueueFamilyIndex(graphicsFamily);
    context.SetPresentQueueFamilyIndex(presentFamily);

    auto [device, graphicsQueue, presentQueue] =
        CreateLogicalDevice(graphicsFamily, presentFamily, physicalDevice);
    context.SetDevice(device);
    context.SetGraphicsQueue(graphicsQueue);
    context.SetPresentQueue(presentQueue);

    auto [swapchain, imageFormat, extent, images, imageViews] = CreateSwapchain(
        physicalDevice, device, surface, window, graphicsFamily, presentFamily);

    const int imageCount = static_cast<int>(images.size());

    context.SetSwapchain(swapchain);
    context.SetSwapchainImageFormat(imageFormat);
    context.SetSwapchainExtent(extent);
    context.SetSwapchainImages(images);

    VkRenderPass renderPass = CreateRenderPass(device, imageFormat);
    context.SetRenderPass(renderPass);

    VkCommandPool commandPool = CreateCommandBufferPool(device, graphicsFamily);
    auto commandBuffers = CreateCommandBuffers(device, commandPool, imageCount);
    context.SetCommandPool(commandPool);
    context.SetCommandBuffers(commandBuffers);

    auto framebuffers =
        CreateFramebuffers(device, renderPass, imageViews, extent);
    context.SetFramebuffers(framebuffers);
    context.SetSwapchainImageViews(imageViews);
}
} // namespace Vulkan