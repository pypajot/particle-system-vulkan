#pragma once

#include <vulkan/vulkan.h>
#include <string>

class ParticleSystemImages
{
    public:
        ParticleSystemImages();
        ParticleSystemImages(const ParticleSystemImages &other) = delete;
        ParticleSystemImages &operator=(const ParticleSystemImages &other) = delete;
        ~ParticleSystemImages();

        void createTextureSampler();
        void createShadowMapTextureSampler();

        void createColorResources();
        void createDepthResources();

        void createTextureImage(VkCommandBuffer commandBuffer);
        void createTextureImageView();
        
        void createBumpImage(VkCommandBuffer commandBuffer);
        void createBumpImageView();

        void createDepthMap();

    private:
        const std::string TEXTURE_PATH = "textures/moon_diffuse.png";
        const std::string RING_TEXTURE_PATH = "textures/saturn_ring_texture.png";

        VkDevice device;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;
        VkSampleCountFlagBits msaaSamples;
        VkPhysicalDeviceMemoryProperties memProperties;

        VkImage colorImage;
        VkDeviceMemory colorImageMemory;
        VkImageView colorImageView;

        VkImage depthImage;
        VkDeviceMemory depthImageMemory;
        VkImageView depthImageView;

        VkImage textureImage;
        VkDeviceMemory textureImageMemory;
        VkImageView textureImageView;

        VkImage depthMap;
        VkDeviceMemory depthMapMemory;
        VkImageView depthMapView;

        VkSampler textureSampler;
        VkSampler shadowMapTextureSampler;

        VkImage bumpImage;
        VkDeviceMemory bumpImageMemory;
        VkImageView bumpImageView;
};