#pragma once

#include <vulkan/vulkan.hpp>

#define VK_CHECK(result) VulkanCheckError(result, __FILE__, __LINE__)

static void VulkanCheckError(VkResult result, const char *file, int lineNumber);

uint32_t findMemoryType(VkPhysicalDeviceMemoryProperties memProperties, uint32_t typeFilter, VkMemoryPropertyFlags properties);

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
    VkImage& image,
    VkDeviceMemory& imageMemory
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

VkCommandBuffer beginSingleTimeCommands(VkDevice device, VkCommandPool commandPool);
void endSingleTimeCommands(VkDevice device, VkCommandBuffer commandBuffer, VkQueue submitQueue, VkCommandPool commandPool);
VkFormat findSupportedFormat(VkPhysicalDevice physicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
