#pragma once

#include <vulkan/vulkan.hpp>
#include "QueueFamilyIndices.hpp"
#include "Swapchain.hpp"

#define VK_CHECK(result) VulkanCheckError(result, __FILE__, __LINE__)

void VulkanCheckError(VkResult result, const char *file, int lineNumber);

std::vector<char> readFile(const std::string& filename);

VkShaderModule createShaderModule(VkDevice device, const std::vector<char>& code);
uint32_t findMemoryType(VkPhysicalDeviceMemoryProperties memProperties, uint32_t typeFilter, VkMemoryPropertyFlags properties);
SwapchainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

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
);

void createTextureImage
(
    std::string path,
    VkImage &targetImage,
    VkDeviceMemory &targetImageMemory,
    VkDevice device,
    VkPhysicalDeviceMemoryProperties memProperties,
    VkQueue graphicsQueue,
    VkCommandPool commandPool
);

void transition_image_layout
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
);

VkImageView createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
void createBuffer(VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory, VkPhysicalDeviceMemoryProperties memProperties);
void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue);
void copyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
VkCommandBuffer beginSingleTimeCommands(VkDevice device, VkCommandPool commandPool);
void endSingleTimeCommands(VkDevice device, VkCommandBuffer commandBuffer, VkQueue submitQueue, VkCommandPool commandPool);
VkFormat findSupportedFormat(VkPhysicalDevice physicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
VkFormat findDepthFormat(VkPhysicalDevice physicalDevice);
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);