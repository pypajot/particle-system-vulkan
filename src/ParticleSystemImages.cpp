#include <vector>

#include "ParticleSystemUtils.hpp"
#include "ParticleSystemImages.hpp"
#include "ParticleSystemApplication.hpp"
#include "Particle.hpp"

#ifndef STB_IMAGE_IMPLEMENTATION
    #define STB_IMAGE_IMPLEMENTATION
#endif

#include <stb/stb_image.h>

ParticleSystemImages::ParticleSystemImages() {}

ParticleSystemImages::~ParticleSystemImages() {}


void ParticleSystemImages::createColorResources()
{
    VkFormat colorFormat = swapChainImageFormat;

    createImage(device, memProperties, swapChainExtent.width, swapChainExtent.height, msaaSamples, colorFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, colorImage, colorImageMemory);
    colorImageView = createImageView(device, colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);
}

void ParticleSystemImages::createDepthResources()
{
    VkFormat depthFormat = findDepthFormat();
    createImage(device, memProperties, swapChainExtent.width, swapChainExtent.height, msaaSamples, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
    depthImageView = createImageView(device, depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
}

void ParticleSystemImages::createDepthMap()
{
    VkFormat depthFormat = findDepthFormat();
    createImage
    (
        device,
        memProperties,
        SHADOW_MAP_WIDTH,
        SHADOW_MAP_HEIGHT,
        VK_SAMPLE_COUNT_1_BIT,
        depthFormat,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        depthMap,
        depthMapMemory
    );
    depthMapView = createImageView(device, depthMap, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
    // transitionImageLayout(depthMap, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

void ParticleSystemImages::createTextureImage(VkCommandBuffer commandBuffer)
{
    int texWidth, texHeight, texChannels;

    stbi_uc* pixels = stbi_load(TEXTURE_PATH.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels)
        throw std::runtime_error("failed to load texture image!");

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    
    createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT, stagingBuffer, stagingBufferMemory);

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
        textureImage,
        textureImageMemory
    );

    transition_image_layout
    (
        commandBuffer,
        textureImage,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        0,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT
    );
    // transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(commandBuffer, stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
    // transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    transition_image_layout
    (
        commandBuffer,
        textureImage,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_ACCESS_SHADER_READ_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT
    );
    endSingleTimeCommands(device, commandBuffer, graphicsAndComputeQueue);
    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void ParticleSystemImages::createTextureImageView()
{
    textureImageView = createImageView(device, textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
}

void ParticleSystemImages::createTextureSampler()
{
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);

    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

    samplerInfo.unnormalizedCoordinates = VK_FALSE;

    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    VK_CHECK(vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler));
}

void ParticleSystemImages::createShadowMapTextureSampler()
{
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);

    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    // samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

    samplerInfo.unnormalizedCoordinates = VK_FALSE;

    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    VK_CHECK(vkCreateSampler(device, &samplerInfo, nullptr, &shadowMapTextureSampler));
}

void ParticleSystemImages::createBumpImage(VkCommandBuffer commandBuffer)
{
    int texWidth, texHeight, texChannels;

    stbi_uc* pixels = stbi_load(RING_TEXTURE_PATH.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels)
        throw std::runtime_error("failed to load bump image!");

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    
    createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT, stagingBuffer, stagingBufferMemory);

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
        bumpImage,
        bumpImageMemory
    );

    transition_image_layout
    (
        commandBuffer,
        bumpImage,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        0,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT
    );
    copyBufferToImage(commandBuffer, stagingBuffer, bumpImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
    transition_image_layout
    (
        commandBuffer,
        bumpImage,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_ACCESS_SHADER_READ_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT
    );

    endSingleTimeCommands(device, commandBuffer, graphicsAndComputeQueue);
    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void ParticleSystemImages::createBumpImageView()
{
    bumpImageView = createImageView(device, bumpImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
}