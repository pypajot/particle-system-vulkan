#include "ParticleSystemUtils.hpp"

#include <fstream>
#include <unordered_map>

#ifndef STB_IMAGE_IMPLEMENTATION
    #define STB_IMAGE_IMPLEMENTATION
#endif

#include <stb/stb_image.h>

static const std::unordered_map<VkResult, std::string> VulkanErrorToString
{
    {VK_SUCCESS, "Command successfully completed"},
    {VK_NOT_READY, "A fence or query has not yet completed"},
    {VK_TIMEOUT, "A wait operation has not completed in the specified time"},
    {VK_EVENT_SET, "An event is signaled"},
    {VK_EVENT_RESET, "An event is unsignaled"},
    {VK_INCOMPLETE, "A return array was too small for the result"},
    {VK_SUBOPTIMAL_KHR, "A swapchain no longer matches the surface properties exactly, but can still be used to present to the surface successfully"},
    {VK_THREAD_IDLE_KHR, "A deferred operation is not complete but there is currently no work for this thread to do at the time of this call"},
    {VK_THREAD_DONE_KHR, "A deferred operation is not complete but there is no work remaining to assign to additional threads"},
    {VK_OPERATION_DEFERRED_KHR, "A deferred operation was requested and at least some of the work was deferred"},
    {VK_OPERATION_NOT_DEFERRED_KHR, "A deferred operation was requested and no operations were deferred"},
    {VK_PIPELINE_COMPILE_REQUIRED, "A requested pipeline creation would have required compilation, but the application requested compilation to not be performed"},
    {VK_ERROR_OUT_OF_HOST_MEMORY, "A host memory allocation has failed"},
    {VK_ERROR_OUT_OF_DEVICE_MEMORY, "A device memory allocation has failed"},
    {VK_ERROR_INITIALIZATION_FAILED, "Initialization of an object could not be completed for implementation-specific reasons"},
    {VK_ERROR_DEVICE_LOST, "The logical or physical device has been lost"},
    {VK_ERROR_MEMORY_MAP_FAILED, "Mapping of a memory object has failed"},
    {VK_ERROR_LAYER_NOT_PRESENT, "A requested layer is not present or could not be loaded"},
    {VK_ERROR_EXTENSION_NOT_PRESENT, "A requested extension is not supported"},
    {VK_ERROR_FEATURE_NOT_PRESENT, "A requested feature is not supported"},
    {VK_ERROR_INCOMPATIBLE_DRIVER, "The requested version of Vulkan is not supported by the driver or is otherwise incompatible for implementation-specific reasons"},
    {VK_ERROR_TOO_MANY_OBJECTS, "Too many objects of the type have already been created"},
    {VK_ERROR_FORMAT_NOT_SUPPORTED, "A requested format is not supported on this device"},
    {VK_ERROR_FRAGMENTED_POOL, "A pool allocation has failed due to fragmentation of the pool’s memory"},
    {VK_ERROR_SURFACE_LOST_KHR, "A surface is no longer available"},
    {VK_ERROR_NATIVE_WINDOW_IN_USE_KHR, "The requested window is already in use by Vulkan or another API in a manner which prevents it from being used again"},
    {VK_ERROR_OUT_OF_DATE_KHR, "A surface has changed in such a way that it is no longer compatible with the swapchain, and further presentation requests using the swapchain will fail"},
    {VK_ERROR_INCOMPATIBLE_DISPLAY_KHR, "The display used by a swapchain does not use the same presentable image layout, or is incompatible in a way that prevents sharing an image"},
    {VK_ERROR_INVALID_SHADER_NV, "One or more shaders failed to compile or link"},
    {VK_ERROR_OUT_OF_POOL_MEMORY, "A pool memory allocation has failed"},
    {VK_ERROR_INVALID_EXTERNAL_HANDLE, "An external handle is not a valid handle of the specified type"},
    {VK_ERROR_FRAGMENTATION, "A descriptor pool creation has failed due to fragmentation"},
    {VK_ERROR_INVALID_DEVICE_ADDRESS_EXT, "A buffer creation failed because the requested address is not available"},
    {VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS, "A buffer creation or memory allocation failed because the requested address is not available A shader group handle assignment failed because the requested shader group handle information is no longer valid"},
    {VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT, "An operation on a swapchain created with VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT failed as it did not have exclusive full-screen access"},
    {VK_ERROR_COMPRESSION_EXHAUSTED_EXT, "An image creation failed because internal resources required for compression are exhausted"},
    {VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR, "The requested VkImageUsageFlags are not supported"},
    {VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR, "The requested video picture layout is not supported"},
    {VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR, "A video profile operation specified via VkVideoProfileInfoKHR::videoCodecOperation is not supported"},
    {VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR, "Format parameters in a requested VkVideoProfileInfoKHR chain are not supported"},
    {VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR, "Codec-specific parameters in a requested VkVideoProfileInfoKHR chain are not supported"},
    {VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR, "The specified video Std header version is not supported"},
    {VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR, "The specified Video Std parameters do not adhere to the syntactic or semantic requirements of the used video compression standard, or values derived from parameters according to the rules defined by the used video compression standard do not adhere to the capabilities of the video compression standard or the implementation"},
    {VK_ERROR_UNKNOWN, "An unknown error has occurred; either the application has provided invalid input, or an implementation failure has occurred"}
};

void VulkanCheckError(VkResult result, const char *file, int lineNumber)
{
    if (result == VK_SUCCESS)
        return;
    
    std::string line = std::to_string(lineNumber);
    std::string errorString = "Error: " + VulkanErrorToString.at(result) + "\nin file " + file + "\nline: " + line + "\n";
    throw std::runtime_error(errorString);
}

std::vector<char> readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
        throw std::runtime_error("Failed to open file: " + filename);

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

SwapchainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    SwapchainSupportDetails details;
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

VkShaderModule createShaderModule(VkDevice device, const std::vector<char>& code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
    VkShaderModule shaderModule;

    VK_CHECK(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule));
    
    return shaderModule;
}

void createBuffer(VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory, VkPhysicalDeviceMemoryProperties memProperties)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK(vkCreateBuffer(device, &bufferInfo, nullptr, &buffer));

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memProperties, memRequirements.memoryTypeBits, properties);

    vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory);
    vkBindBufferMemory(device, buffer, bufferMemory, 0);

}

void copyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
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

void createTextureImage
(
    std::string path,
    VkImage &targetImage,
    VkDeviceMemory &targetImageMemory,
    VkDevice device,
    VkPhysicalDeviceMemoryProperties memProperties,
    VkQueue graphicsQueue,
    VkCommandPool commandPool
)
{
    int texWidth, texHeight, texChannels;

    stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels)
        throw std::runtime_error("failed to load texture image!");

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    
    createBuffer(device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT, stagingBuffer, stagingBufferMemory, memProperties);

    void* data;
    VK_CHECK(vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data));
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(device, stagingBufferMemory);
    
    stbi_image_free(pixels);

    createImage
    (
        device,
        memProperties,
        texWidth,
        texHeight,
        VK_SAMPLE_COUNT_1_BIT,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        targetImage,
        targetImageMemory
    );

    VkCommandBuffer commandBuffer = beginSingleTimeCommands(device, commandPool);

    transitionImageLayout
    (
        commandBuffer,
        targetImage,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        0,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT
    );
    copyBufferToImage(commandBuffer, stagingBuffer, targetImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

    transitionImageLayout
    (
        commandBuffer,
        targetImage,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_ACCESS_SHADER_READ_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT
    );
    endSingleTimeCommands(device, commandBuffer, graphicsQueue, commandPool);
    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);
}

uint32_t findMemoryType(VkPhysicalDeviceMemoryProperties memProperties, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;
    }

    throw std::runtime_error("Failed to find suitable memory type.");
}

void createImage
(
    VkDevice device,
    VkPhysicalDeviceMemoryProperties memProperties,
    uint32_t width,
    uint32_t height,
    VkSampleCountFlagBits numSamples,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage, 
    VkMemoryPropertyFlags properties,
    VkImage &image,
    VkDeviceMemory &imageMemory
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
    allocInfo.memoryTypeIndex = findMemoryType(memProperties, memRequirements.memoryTypeBits, properties);

    VK_CHECK(vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory));
    VK_CHECK(vkBindImageMemory(device, image, imageMemory, 0));
}

VkImageView createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;

    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    VK_CHECK(vkCreateImageView(device, &viewInfo, nullptr, &imageView));

    return imageView;
}

VkCommandBuffer beginSingleTimeCommands(VkDevice device, VkCommandPool commandPool)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    VK_CHECK(vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer));

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));

    return commandBuffer;
}

void endSingleTimeCommands(VkDevice device, VkCommandBuffer commandBuffer, VkQueue submitQueue, VkCommandPool commandPool)
{
    VK_CHECK(vkEndCommandBuffer(commandBuffer));

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    VK_CHECK(vkQueueSubmit(submitQueue, 1, &submitInfo, VK_NULL_HANDLE));
    VK_CHECK(vkQueueWaitIdle(submitQueue));

    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(device, commandPool);

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(device, commandBuffer, graphicsQueue, commandPool);
}

VkFormat findSupportedFormat(VkPhysicalDevice physicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
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

VkFormat findDepthFormat(VkPhysicalDevice physicalDevice)
{
    return findSupportedFormat
    (
        physicalDevice,
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

void transitionImageLayout
(
    VkCommandBuffer commandBuffer,
    VkImage targetImage,
    VkImageLayout oldLayout,
    VkImageLayout newLayout,
    VkAccessFlags2 srcAccessMask,
    VkAccessFlags2 dstAccessMask,
    VkPipelineStageFlags2 srcStageMask,
    VkPipelineStageFlags2 dstStageMask,
    uint imageAspectFlags
)
{
    VkImageMemoryBarrier2 barrier{};
    
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    barrier.pNext = nullptr;
    barrier.srcStageMask = srcStageMask;
    barrier.srcAccessMask = srcAccessMask;
    barrier.dstStageMask = dstStageMask;
    barrier.dstAccessMask = dstAccessMask;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.image = targetImage;
    barrier.subresourceRange =
    {
        .aspectMask = imageAspectFlags,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1
    };

    VkDependencyInfo dependencyInfo{};
    
    dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependencyInfo.pNext = nullptr;
    dependencyInfo.dependencyFlags = {};
    dependencyInfo.imageMemoryBarrierCount = 1;
    dependencyInfo.pImageMemoryBarriers = &barrier;

    vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);

}

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
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
