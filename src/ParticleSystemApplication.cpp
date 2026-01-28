#include "ParticleSystemApplication.hpp"
#include "Particle.hpp"
#include "ParticleSystemUtils.hpp"

#include <cstring>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <limits>
#include <algorithm>
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>

#ifndef STB_IMAGE_IMPLEMENTATION
    #define STB_IMAGE_IMPLEMENTATION
#endif

#include <stb/stb_image.h>

#ifndef TINYOBJLOADER_IMPLEMENTATION
    #define TINYOBJLOADER_IMPLEMENTATION
#endif

#include <tiny_obj_loader.h>


void testFun()
{
    std::cout << "test\n";
}

ParticleSystemApplication::ParticleSystemApplication() {}

ParticleSystemApplication::~ParticleSystemApplication() {}

void ParticleSystemApplication::initWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(BASE_WIN_WIDTH, BASE_WIN_HEIGHT, "Vulkan window", nullptr, nullptr);

    if (window == nullptr)
        throw std::runtime_error("Failed to create window.");
}

bool ParticleSystemApplication::checkValidationLayerSupport()
{
    uint32_t layerCount;
    VK_CHECK(vkEnumerateInstanceLayerProperties(&layerCount, nullptr));

    std::vector<VkLayerProperties> availableLayers(layerCount);
    VK_CHECK(vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data()));

    for (const char* layerName : validationLayers)
    {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
            return false;
    }
    return true;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback
(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData
)
{
    (void)messageType;
    (void)pUserData;

    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

std::vector<const char*> ParticleSystemApplication::getRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers)
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

    return extensions;
}

void ParticleSystemApplication::createInstance()
{
    if (enableValidationLayers && !checkValidationLayerSupport())
        throw std::runtime_error("Validation layers requested but not available.");

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Particle system";
    appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 3, 0);
    appInfo.pEngineName = "Particle engine";
    appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 3, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    
    std::vector<const char*> extensions = getRequiredExtensions();
    
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    }
    else 
    {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
        createInfo.enabledLayerCount = 0;

    VK_CHECK(vkCreateInstance(&createInfo, nullptr, &instance));
}

VkResult CreateDebugUtilsMessengerEXT
(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger
)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    else
        return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void ParticleSystemApplication::setupDebugMessenger()
{
    if (!enableValidationLayers)
        return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    VK_CHECK(CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger));
}

QueueFamilyIndices ParticleSystemApplication::findQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies)
    {
        if (queueFamily.queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT))
            indices.graphicsAndComputeFamily = i;

        VkBool32 presentSupport = false;
        VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport));

        if (presentSupport)
            indices.presentFamily = i;

        if (indices.isComplete())
            break;

        i++;
    }
    return indices;
}


bool ParticleSystemApplication::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount;
    VK_CHECK(vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr));

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    VK_CHECK(vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data()));

    std::unordered_set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions)
        requiredExtensions.erase(extension.extensionName);

    return requiredExtensions.empty();
}

SwapChainSupportDetails ParticleSystemApplication::querySwapChainSupport(VkPhysicalDevice device)
{
    SwapChainSupportDetails details;
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities));

    uint32_t formatCount;
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr));
    if (formatCount != 0)
    {
        details.formats.resize(formatCount);
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data()));
    }

    uint32_t presentModeCount;
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr));
    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data()));
    }
    return details;
}

VkSurfaceFormatKHR ParticleSystemApplication::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return availableFormat;
    }
    return availableFormats[0];
}

VkPresentModeKHR ParticleSystemApplication::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            return availablePresentMode;
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D ParticleSystemApplication::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        return capabilities.currentExtent;

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    VkExtent2D actualExtent =
    {
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height)
    };

    actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return actualExtent;
}

void ParticleSystemApplication::createSwapChain()
{
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        imageCount = swapChainSupport.capabilities.maxImageCount;
    
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
    uint32_t queueFamilyIndices[] = {indices.graphicsAndComputeFamily.value(), indices.presentFamily.value()};

    // To change for better performance maybe ? , look up ownership for the swap chain images
    if (indices.graphicsAndComputeFamily != indices.presentFamily) 
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        // createInfo.queueFamilyIndexCount = 0;
        // createInfo.pQueueFamilyIndices = nullptr;
    }
    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    // To change for resize window
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VK_CHECK(vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain));

    VK_CHECK(vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr));
    swapChainImages.resize(imageCount);
    VK_CHECK(vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data()));
    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
}

void ParticleSystemApplication::createImage
(
    uint32_t width,
    uint32_t height,
    VkSampleCountFlagBits numSamples,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage, 
    VkMemoryPropertyFlags properties,
    VkImage& image,
    VkDeviceMemory& imageMemory
)
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = numSamples;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK(vkCreateImage(device, &imageInfo, nullptr, &image));

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    VK_CHECK(vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory));
    VK_CHECK(vkBindImageMemory(device, image, imageMemory, 0));
}

void ParticleSystemApplication::createImageViews()
{
    swapChainImageViews.resize(swapChainImages.size());
    for (uint i = 0; i < swapChainImageViews.size(); i++)
    {
        swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
    }
}

void ParticleSystemApplication::copyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {width, height, 1};

    vkCmdCopyBufferToImage
    (
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );
}

// void ParticleSystemApplication::createTextureImage(VkCommandBuffer commandBuffer)
// {
//     int texWidth, texHeight, texChannels;

//     stbi_uc* pixels = stbi_load(TEXTURE_PATH.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
//     VkDeviceSize imageSize = texWidth * texHeight * 4;

//     if (!pixels)
//         throw std::runtime_error("failed to load texture image!");

//     VkBuffer stagingBuffer;
//     VkDeviceMemory stagingBufferMemory;
    
//     createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT, stagingBuffer, stagingBufferMemory);

//     void* data;
//     VK_CHECK(vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data));
//     memcpy(data, pixels, static_cast<size_t>(imageSize));
//     vkUnmapMemory(device, stagingBufferMemory);
    
//     stbi_image_free(pixels);

//     createImage
//     (
//         texWidth,
//         texHeight,
//         VK_SAMPLE_COUNT_1_BIT,
//         VK_FORMAT_R8G8B8A8_SRGB,
//         VK_IMAGE_TILING_OPTIMAL,
//         VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
//         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
//         textureImage,
//         textureImageMemory
//     );

//     transition_image_layout
//     (
//         commandBuffer,
//         textureImage,
//         VK_IMAGE_LAYOUT_UNDEFINED,
//         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
//         0,
//         VK_ACCESS_TRANSFER_WRITE_BIT,
//         VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
//         VK_PIPELINE_STAGE_TRANSFER_BIT,
//         VK_IMAGE_ASPECT_COLOR_BIT
//     );
//     // transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
//     copyBufferToImage(commandBuffer, stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
//     // transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

//     transition_image_layout
//     (
//         commandBuffer,
//         textureImage,
//         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
//         VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
//         VK_ACCESS_TRANSFER_WRITE_BIT,
//         VK_ACCESS_SHADER_READ_BIT,
//         VK_PIPELINE_STAGE_TRANSFER_BIT,
//         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
//         VK_IMAGE_ASPECT_COLOR_BIT
//     );
//     endSingleTimeCommands(commandBuffer, graphicsAndComputeQueue);
//     vkDestroyBuffer(device, stagingBuffer, nullptr);
//     vkFreeMemory(device, stagingBufferMemory, nullptr);
// }

// void ParticleSystemApplication::createTextureImageView()
// {
//     textureImageView = createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
// }

// void ParticleSystemApplication::createTextureSampler()
// {
//     VkSamplerCreateInfo samplerInfo{};
//     samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
//     samplerInfo.magFilter = VK_FILTER_LINEAR;
//     samplerInfo.minFilter = VK_FILTER_LINEAR;
//     samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
//     samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
//     samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

//     VkPhysicalDeviceProperties properties{};
//     vkGetPhysicalDeviceProperties(physicalDevice, &properties);

//     samplerInfo.anisotropyEnable = VK_TRUE;
//     samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
//     samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

//     samplerInfo.unnormalizedCoordinates = VK_FALSE;

//     samplerInfo.compareEnable = VK_FALSE;
//     samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

//     samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
//     samplerInfo.mipLodBias = 0.0f;
//     samplerInfo.minLod = 0.0f;
//     samplerInfo.maxLod = 0.0f;

//     VK_CHECK(vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler));
// }

// void ParticleSystemApplication::createShadowMapTextureSampler()
// {
//     VkSamplerCreateInfo samplerInfo{};
//     samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
//     samplerInfo.magFilter = VK_FILTER_LINEAR;
//     samplerInfo.minFilter = VK_FILTER_LINEAR;
//     samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
//     samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
//     samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
//     samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

//     VkPhysicalDeviceProperties properties{};
//     vkGetPhysicalDeviceProperties(physicalDevice, &properties);

//     samplerInfo.anisotropyEnable = VK_TRUE;
//     samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
//     // samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

//     samplerInfo.unnormalizedCoordinates = VK_FALSE;

//     samplerInfo.compareEnable = VK_FALSE;
//     samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

//     samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
//     samplerInfo.mipLodBias = 0.0f;
//     samplerInfo.minLod = 0.0f;
//     samplerInfo.maxLod = 0.0f;

//     VK_CHECK(vkCreateSampler(device, &samplerInfo, nullptr, &shadowMapTextureSampler));
// }

// void ParticleSystemApplication::createBumpImage(VkCommandBuffer commandBuffer)
// {
//     int texWidth, texHeight, texChannels;

//     stbi_uc* pixels = stbi_load(RING_TEXTURE_PATH.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
//     VkDeviceSize imageSize = texWidth * texHeight * 4;

//     if (!pixels)
//         throw std::runtime_error("failed to load bump image!");

//     VkBuffer stagingBuffer;
//     VkDeviceMemory stagingBufferMemory;
    
//     createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT, stagingBuffer, stagingBufferMemory);

//     void* data;
//     VK_CHECK(vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data));
//     memcpy(data, pixels, static_cast<size_t>(imageSize));
//     vkUnmapMemory(device, stagingBufferMemory);
    
//     stbi_image_free(pixels);

//     createImage
//     (
//         texWidth,
//         texHeight,
//         VK_SAMPLE_COUNT_1_BIT,
//         VK_FORMAT_R8G8B8A8_SRGB,
//         VK_IMAGE_TILING_OPTIMAL,
//         VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
//         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
//         bumpImage,
//         bumpImageMemory
//     );

//     transition_image_layout
//     (
//         commandBuffer,
//         bumpImage,
//         VK_IMAGE_LAYOUT_UNDEFINED,
//         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
//         0,
//         VK_ACCESS_TRANSFER_WRITE_BIT,
//         VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
//         VK_PIPELINE_STAGE_TRANSFER_BIT,
//         VK_IMAGE_ASPECT_COLOR_BIT
//     );
//     copyBufferToImage(commandBuffer, stagingBuffer, bumpImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
//     transition_image_layout
//     (
//         commandBuffer,
//         bumpImage,
//         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
//         VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
//         VK_ACCESS_TRANSFER_WRITE_BIT,
//         VK_ACCESS_SHADER_READ_BIT,
//         VK_PIPELINE_STAGE_TRANSFER_BIT,
//         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
//         VK_IMAGE_ASPECT_COLOR_BIT
//     );

//     endSingleTimeCommands(commandBuffer, graphicsAndComputeQueue);
//     vkDestroyBuffer(device, stagingBuffer, nullptr);
//     vkFreeMemory(device, stagingBufferMemory, nullptr);
// }

// void ParticleSystemApplication::createBumpImageView()
// {
//     bumpImageView = createImageView(bumpImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
// }

VkSampleCountFlagBits ParticleSystemApplication::getMaxUsableSampleCount()
{
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

    VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
    if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
    if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
    if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
    if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
    if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
    if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

    return VK_SAMPLE_COUNT_1_BIT;
}

int ParticleSystemApplication::rateDeviceSuitability(VkPhysicalDevice device)
{
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    int score = 0;

    QueueFamilyIndices indices = findQueueFamilies(device);
    if (!indices.isComplete())
        return 0;

    bool swapChainAdequate = false;
    bool extensionsSupported = checkDeviceExtensionSupport(device);

    if (extensionsSupported)
    {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    if (!swapChainAdequate)
        return 0;

    // Discrete GPUs have a significant performance advantage
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        score += 1000;

    // Maximum possible size of textures affects graphics quality
    score += deviceProperties.limits.maxImageDimension2D;

    // Application can't function without geometry shaders
    if (!deviceFeatures.geometryShader)
        return 0;

    if (!deviceFeatures.samplerAnisotropy)
        return 0;

    return score;
}

void ParticleSystemApplication::pickPhysicalDevice()
{
    uint32_t deviceCount = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr));

    if (deviceCount == 0)
        throw std::runtime_error("Failed to find GPUs with Vulkan support.");

    std::vector<VkPhysicalDevice> devices(deviceCount);
    VK_CHECK(vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data()));

    std::multimap<int, VkPhysicalDevice> candidates;

    for (const auto& device : devices)
    {
        int score = rateDeviceSuitability(device);
        candidates.insert(std::make_pair(score, device));
    }

    if (candidates.rbegin()->first > 0)
    {
        physicalDevice = candidates.rbegin()->second;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
        msaaSamples = getMaxUsableSampleCount();
    }
    else
        throw std::runtime_error("Failed to find a suitable GPU.");
}

void ParticleSystemApplication::createLogicalDevice()
{
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    std::unordered_set<uint32_t> uniqueQueueFamilies =
    {
        indices.graphicsAndComputeFamily.value(),
        indices.presentFamily.value()
    };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily: uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }
    
    VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeature{};

    dynamicRenderingFeature.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
    dynamicRenderingFeature.dynamicRendering = VK_TRUE;

    VkPhysicalDeviceSynchronization2Features synchronisation2Feature{};

    synchronisation2Feature.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR;
    synchronisation2Feature.pNext = &dynamicRenderingFeature;
    synchronisation2Feature.synchronization2 = true;

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo{};

    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = &synchronisation2Feature;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
        createInfo.enabledLayerCount = 0;

    VK_CHECK(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device));

    vkGetDeviceQueue(device, indices.graphicsAndComputeFamily.value(), 0, &graphicsAndComputeQueue);
    vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

void ParticleSystemApplication::createSurface()
{
    VK_CHECK(glfwCreateWindowSurface(instance, window, nullptr, &surface));
}

void ParticleSystemApplication::createCommandPool()
{
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsAndComputeFamily.value();

    VK_CHECK(vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool));
}

void ParticleSystemApplication::createCommandBuffer()
{
    commandBuffer.resize(NUMBER_OF_FRAMES_IN_FLIGHT);
    computeCommandBuffer.resize(NUMBER_OF_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = NUMBER_OF_FRAMES_IN_FLIGHT;

    VK_CHECK(vkAllocateCommandBuffers(device, &allocInfo, commandBuffer.data()));

    VK_CHECK(vkAllocateCommandBuffers(device, &allocInfo, computeCommandBuffer.data()));

}

void ParticleSystemApplication::recordShadowMapRendering(VkCommandBuffer commandBuffer)
{
    VkClearValue clearValue{};
    clearValue.depthStencil = {1.0f, 0};

    VkRenderingAttachmentInfo shadowPassDepthAttachmentInfo{};
    shadowPassDepthAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    shadowPassDepthAttachmentInfo.imageView = depthMapView;
    shadowPassDepthAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
    shadowPassDepthAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    shadowPassDepthAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    shadowPassDepthAttachmentInfo.clearValue = clearValue;

    VkRenderingInfo shadowPassRenderingInfo{};
    shadowPassRenderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    shadowPassRenderingInfo.renderArea = { .offset = { 0, 0 }, .extent = {SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT} };
    shadowPassRenderingInfo.layerCount = 1;
    shadowPassRenderingInfo.colorAttachmentCount = 0;
    shadowPassRenderingInfo.pColorAttachments = nullptr;
    shadowPassRenderingInfo.pDepthAttachment = &shadowPassDepthAttachmentInfo;

    vkCmdBeginRendering(commandBuffer, &shadowPassRenderingInfo);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shadowMapModelPipeline);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(SHADOW_MAP_WIDTH);
    viewport.height = static_cast<float>(SHADOW_MAP_HEIGHT);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = {SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT};
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    VkBuffer vertexBuffers[] = {vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shadowMapModelPipelineLayout, 0, 1, &shadowMapDescriptorSets[currentFrame], 0, nullptr);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shadowMapParticlePipeline);

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vertexBuffers[0] = shaderStorageBuffers[currentFrame];
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shadowMapParticlePipelineLayout, 0, 1, &shadowMapDescriptorSets[currentFrame], 0, nullptr);
    vkCmdDraw(commandBuffer, PARTICLE_NUMBER, 1, 0, 0);
    vkCmdEndRendering(commandBuffer);
}

void ParticleSystemApplication::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

    transition_image_layout
    (
        commandBuffer,
        depthMap,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
        VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
        VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
        VK_IMAGE_ASPECT_DEPTH_BIT
    );

    recordShadowMapRendering(commandBuffer);

    transition_image_layout
    (
        commandBuffer,
        depthMap,
        VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        VK_ACCESS_2_SHADER_SAMPLED_READ_BIT,
        VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
        VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
        VK_IMAGE_ASPECT_DEPTH_BIT
    );

    transition_image_layout
    (
        commandBuffer,
        swapChainImages[imageIndex],
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        {},
        VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT
    );

    transition_image_layout
    (
        commandBuffer,
        depthImage,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
        VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
        VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
        VK_IMAGE_ASPECT_DEPTH_BIT
    );

    transition_image_layout
    (
        commandBuffer,
        colorImage,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT
    );

    VkRenderingAttachmentInfo colorAttachmentInfo{};
    colorAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachmentInfo.imageView = colorImageView;
    colorAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachmentInfo.clearValue = clearValues[0];

    colorAttachmentInfo.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
    colorAttachmentInfo.resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachmentInfo.resolveImageView = swapChainImageViews[imageIndex];

    VkRenderingAttachmentInfo depthAttachmentInfo{};
    depthAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    depthAttachmentInfo.imageView = depthImageView;
    depthAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
    depthAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachmentInfo.clearValue = clearValues[1];

    VkRenderingInfo renderingInfo{};
    renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renderingInfo.renderArea = { .offset = { 0, 0 }, .extent = swapChainExtent };
    renderingInfo.layerCount = 1;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &colorAttachmentInfo;
    renderingInfo.pDepthAttachment = &depthAttachmentInfo;

    vkCmdBeginRendering(commandBuffer, &renderingInfo);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapChainExtent.width);
    viewport.height = static_cast<float>(swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    VkBuffer vertexBuffers[] = {vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, particleGraphicsPipeline);

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vertexBuffers[0] = shaderStorageBuffers[currentFrame];

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, particleGraphicsPipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr);
    vkCmdDraw(commandBuffer, PARTICLE_NUMBER, 1, 0, 0);

    vkCmdEndRendering(commandBuffer);

    transition_image_layout
    (
        commandBuffer,
        swapChainImages[imageIndex],
        VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        VK_ACCESS_2_NONE_KHR,
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_2_NONE_KHR,
        VK_IMAGE_ASPECT_COLOR_BIT
    );

    VK_CHECK(vkEndCommandBuffer(commandBuffer));
}

void ParticleSystemApplication::createSyncObjects()
{
    imageAvailableSemaphore.resize(NUMBER_OF_FRAMES_IN_FLIGHT);
    renderFinishedSemaphore.resize(swapChainImages.size());
    inFlightFence.resize(NUMBER_OF_FRAMES_IN_FLIGHT);

    computeFinishedSemaphore.resize(NUMBER_OF_FRAMES_IN_FLIGHT);
    computeInFlightFences.resize(NUMBER_OF_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (uint i = 0; i < NUMBER_OF_FRAMES_IN_FLIGHT; i++)
    {
        VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphore[i]));
        VK_CHECK(vkCreateFence(device, &fenceInfo, nullptr, &inFlightFence[i]));
        VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &computeFinishedSemaphore[i]));
        VK_CHECK(vkCreateFence(device, &fenceInfo, nullptr, &computeInFlightFences[i]));
    }

    for (uint i = 0; i < swapChainImages.size(); i++)
    {
        VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphore[i]));
    }
}

uint32_t ParticleSystemApplication::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;
    }

    throw std::runtime_error("Failed to find suitable memory type.");
}

void ParticleSystemApplication::initVulkan()
{
    createInstance();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createImageViews();

    createCommandPool();
    createColorResources();
    createDepthResources();  

    loadModel();

    VkCommandBuffer commandBuffer = beginSingleTimeCommands();
    createVertexBuffer(commandBuffer);  
    commandBuffer = beginSingleTimeCommands();
    createIndexBuffer(commandBuffer);

    createStorageBuffers();
    createUniformBuffers();  
    createSceneDataBuffers();
    createShadowMapUniformBuffers();

    commandBuffer = beginSingleTimeCommands();
    createTextureImage(commandBuffer);
    createTextureImageView();
    createTextureSampler();
    createShadowMapTextureSampler();

    commandBuffer = beginSingleTimeCommands();
    createBumpImage(commandBuffer);
    createBumpImageView();

    createDepthMap();
    // createBumpSampler();
    createDescriptorPool();
    createDescriptorSets();

    createCommandBuffer();
    createSyncObjects();
}

void ParticleSystemApplication::updateUniformBuffer(uint32_t currentImage)
{
    float time = glfwGetTime() / 4;
    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0, 1, 0));
    ubo.model = glm::mat4(1.0f);
    ubo.view = glm::lookAt(glm::vec3(5.0f, -10.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), swapChainExtent.width / (float) swapChainExtent.height, 0.1f, 100.0f);
    // ubo.view = glm::lookAt(glm::vec3(10.0f, -2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    // ubo.proj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -20.0f, 20.0f);
    ubo.proj[1][1] *= -1;
    ubo.projViewModel = ubo.proj * ubo.view * ubo.model;

    memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));

    SceneData scene{};
    scene.ambientLight = glm::vec4(0.1f);
    scene.sunlightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    scene.sunlightDirection = glm::vec4(10.0f, 2.0f, 0.0f, 1.0f);

    memcpy(sceneDataBuffersMapped[currentImage], &scene, sizeof(scene));

    ubo.model = glm::mat4(1.0f);
    ubo.view = glm::lookAt(glm::vec3(10.0f, 2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    ubo.proj = glm::ortho(-7.0f, 7.0f, 7.0f, -7.0f, 0.0f, 20.0f);
    // ubo.proj[1][1] *= -1;
    
    ubo.projViewModel = ubo.proj * ubo.view * ubo.model;

    memcpy(shadowMapUniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}



VkFormat ParticleSystemApplication::findSupportedFormat(VkPhysicalDevice physicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    for (VkFormat format : candidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
            return format;
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
            return format;
    }
    throw std::runtime_error("Failed to find supported format.");
}

bool ParticleSystemApplication::hasStencilComponent(VkFormat format)
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

VkFormat ParticleSystemApplication::findDepthFormat()
{
    return findSupportedFormat
    (
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

// void ParticleSystemApplication::createColorResources()
// {
//     VkFormat colorFormat = swapChainImageFormat;

//     createImage(swapChainExtent.width, swapChainExtent.height, msaaSamples, colorFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, colorImage, colorImageMemory);
//     colorImageView = createImageView(colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);
// }

// void ParticleSystemApplication::createDepthResources()
// {
//     VkFormat depthFormat = findDepthFormat();
//     createImage(swapChainExtent.width, swapChainExtent.height, msaaSamples, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
//     depthImageView = createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
// }

// void ParticleSystemApplication::createDepthMap()
// {
//     VkFormat depthFormat = findDepthFormat();
//     createImage
//     (
//         SHADOW_MAP_WIDTH,
//         SHADOW_MAP_HEIGHT,
//         VK_SAMPLE_COUNT_1_BIT,
//         depthFormat,
//         VK_IMAGE_TILING_OPTIMAL,
//         VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
//         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
//         depthMap,
//         depthMapMemory
//     );
//     depthMapView = createImageView(depthMap, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
//     // transitionImageLayout(depthMap, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
// }

void ParticleSystemApplication::updateParticles()
{
    if (particleNeedReset)
    {
        resetParticle();
        particleNeedReset = false;
    }
    else
    {
        VK_CHECK(vkWaitForFences(device, 1, &computeInFlightFences[currentFrame], VK_TRUE, UINT64_MAX));
        VK_CHECK(vkResetFences(device, 1, &computeInFlightFences[currentFrame]));

        VK_CHECK(vkResetCommandBuffer(computeCommandBuffer[currentFrame], 0));
        recordComputeCommandBuffer(computeCommandBuffer[currentFrame], particleUpdatePipeline, particleUpdatePipelineLayout);

        VkSubmitInfo submitInfo{};

        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &computeCommandBuffer[currentFrame];
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &computeFinishedSemaphore[currentFrame];
        VK_CHECK(vkQueueSubmit(graphicsAndComputeQueue, 1, &submitInfo, computeInFlightFences[currentFrame]));
    }
}

void ParticleSystemApplication::resetParticle()
{
    VK_CHECK(vkWaitForFences(device, 1, &computeInFlightFences[currentFrame], VK_TRUE, UINT64_MAX));
    VK_CHECK(vkResetFences(device, 1, &computeInFlightFences[currentFrame]));

    VK_CHECK(vkResetCommandBuffer(computeCommandBuffer[currentFrame], 0));
    recordComputeCommandBuffer(computeCommandBuffer[currentFrame], particleInitPipeline, particleInitPipelineLayout);

    VkSubmitInfo submitInfo{};

    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &computeCommandBuffer[currentFrame];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &computeFinishedSemaphore[currentFrame];

    VK_CHECK(vkQueueSubmit(graphicsAndComputeQueue, 1, &submitInfo, computeInFlightFences[currentFrame]));
}

void ParticleSystemApplication::recordComputeCommandBuffer(VkCommandBuffer commandBuffer, VkPipeline computePipeline, VkPipelineLayout computePipelineLayout)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipelineLayout, 0, 1, &computeDescriptorSets[currentFrame], 0, 0);
    vkCmdDispatch(commandBuffer, PARTICLE_NUMBER / 64, 1, 1);

    VK_CHECK(vkEndCommandBuffer(commandBuffer));
}


void ParticleSystemApplication::drawFrame()
{
    VK_CHECK(vkWaitForFences(device, 1, &inFlightFence[currentFrame], VK_TRUE, UINT64_MAX));
    VK_CHECK(vkResetFences(device, 1, &inFlightFence[currentFrame]));

    uint32_t imageIndex;
    VK_CHECK(vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphore[currentFrame], VK_NULL_HANDLE, &imageIndex));

    updateUniformBuffer(currentFrame);


    VK_CHECK(vkResetCommandBuffer(commandBuffer[currentFrame], 0));
    recordCommandBuffer(commandBuffer[currentFrame], imageIndex);


    VkSubmitInfo submitInfo = {};

    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkSemaphore waitSemaphores[] = {imageAvailableSemaphore[currentFrame], computeFinishedSemaphore[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 2;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer[currentFrame];

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphore[imageIndex]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    VK_CHECK(vkQueueSubmit(graphicsAndComputeQueue, 1, &submitInfo, inFlightFence[currentFrame]));

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    presentInfo.pResults = nullptr;
    VK_CHECK(vkQueuePresentKHR(presentQueue, &presentInfo));

    currentFrame = (currentFrame + 1) % NUMBER_OF_FRAMES_IN_FLIGHT; 
}

void ParticleSystemApplication::mainLoop()
{
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        updateParticles();
        drawFrame();
    }
    VK_CHECK(vkDeviceWaitIdle(device));

}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

    if (func != nullptr)
        func(instance, debugMessenger, pAllocator);

}

void ParticleSystemApplication::cleanup()
{
    vkDestroyBuffer(device, indexBuffer, nullptr);
    vkFreeMemory(device, indexBufferMemory, nullptr);

    vkDestroyBuffer(device, vertexBuffer, nullptr);
    vkFreeMemory(device, vertexBufferMemory, nullptr);

    vkDestroySampler(device, textureSampler, nullptr);
    vkDestroyImageView(device, textureImageView, nullptr);
    vkDestroyImage(device, textureImage, nullptr);
    vkFreeMemory(device, textureImageMemory, nullptr);

    vkDestroyImageView(device, bumpImageView, nullptr);
    vkDestroyImage(device, bumpImage, nullptr);
    vkFreeMemory(device, bumpImageMemory, nullptr);

    for (uint i = 0; i < NUMBER_OF_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroyBuffer(device, shaderStorageBuffers[i], nullptr);
        vkFreeMemory(device, shaderStorageBuffersMemory[i], nullptr);

        vkDestroyBuffer(device, sceneDataBuffers[i], nullptr);
        vkFreeMemory(device, sceneDataBuffersMemory[i], nullptr);
    }

    for (uint i = 0; i < NUMBER_OF_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(device, imageAvailableSemaphore[i], nullptr);
        vkDestroyFence(device, inFlightFence[i], nullptr);

        vkDestroySemaphore(device, computeFinishedSemaphore[i], nullptr);
        vkDestroyFence(device, computeInFlightFences[i], nullptr);
    }
    for (uint i = 0; i < swapChainImages.size(); i++)
    {
        vkDestroySemaphore(device, renderFinishedSemaphore[i], nullptr);
    }
    
    vkDestroyCommandPool(device, commandPool, nullptr);

    // for (auto framebuffer : swapChainFramebuffers)
    //     vkDestroyFramebuffer(device, framebuffer, nullptr);

    for (size_t i = 0; i < NUMBER_OF_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroyBuffer(device, uniformBuffers[i], nullptr);
        vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
    }

    vkDestroyDescriptorPool(device, descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
    vkDestroyDescriptorSetLayout(device, particleInitDescriptorSetLayout, nullptr);
    vkDestroyPipeline(device, graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device, graphicsPipelineLayout, nullptr);
    vkDestroyPipeline(device, particleGraphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device, particleGraphicsPipelineLayout, nullptr);
    vkDestroyPipeline(device, particleInitPipeline, nullptr);
    vkDestroyPipelineLayout(device, particleInitPipelineLayout, nullptr);
    vkDestroyPipeline(device, particleUpdatePipeline, nullptr);
    vkDestroyPipelineLayout(device, particleUpdatePipelineLayout, nullptr);
    // vkDestroyRenderPass(device, renderPass, nullptr);

    vkDestroyImageView(device, depthImageView, nullptr);
    vkDestroyImage(device, depthImage, nullptr);
    vkFreeMemory(device, depthImageMemory, nullptr);

    vkDestroyImageView(device, colorImageView, nullptr);
    vkDestroyImage(device, colorImage, nullptr);
    vkFreeMemory(device, colorImageMemory, nullptr);

    for (auto imageView : swapChainImageViews)
        vkDestroyImageView(device, imageView, nullptr);

    vkDestroySwapchainKHR(device, swapChain, nullptr);
    vkDestroyDevice(device, nullptr);
    
    if (enableValidationLayers)
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);

    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
    glfwDestroyWindow(window);
    glfwTerminate();
}


void ParticleSystemApplication::run()
{
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}