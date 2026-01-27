#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>

#include "ShadowmapPipeline.hpp"
#include "Swapchain.hpp"
#include "Light.hpp"
#include "Camera.hpp"

class RenderingPipeline
{
    private:
        VkDevice device;
        VkPhysicalDeviceMemoryProperties memProperties;
        VkSampleCountFlagBits msaaSamples;
        VkExtent2D swapChainExtent;
        VkFormat swapChainImageFormat;
        VkFormat depthFormat;

        std::vector<VkBuffer> uniformBuffers;
        std::vector<VkDeviceMemory> uniformBuffersMemory;
        std::vector<void*> uniformBuffersMapped;

        std::vector<VkBuffer> sceneDataBuffers;
        std::vector<VkDeviceMemory> sceneDataBuffersMemory;
        std::vector<void*> sceneDataBuffersMapped;

        VkDescriptorSetLayout descriptorSetLayout;
        std::vector<VkDescriptorSet> descriptorSets;

        VkImage colorImage;
        VkDeviceMemory colorImageMemory;
        VkImageView colorImageView;
        
        VkImage depthImage;
        VkDeviceMemory depthImageMemory;
        VkImageView depthImageView;

        VkPipelineLayout modelPipelineLayout;
        VkPipeline modelPipeline;
        
        VkPipelineLayout particlePipelineLayout;
        VkPipeline particlePipeline;

        // ShadowmapPipeline *shadowmapPipeline;

    public:
        void init(VkDevice device, VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties memProperties, VkFormat swapchainImageFormat, VkExtent2D swapChainExtent, VkSampleCountFlagBits msaaSamples);
        void createColorImage();
        void createDepthImage();
        void createUniformBuffers();
        void createSceneDataBuffers();
        void createDescriptorSetLayout();
        void createDescriptorSets
        (
            std::vector<VkBuffer> shadowMapUniformBuffers,
            VkImageView textureImageView,
            VkSampler textureSampler,
            VkImageView depthMapView,
            VkSampler shadowMapTextureSampler,
            VkDescriptorPool descriptorPool
        );
        void createModelPipeline();
        void createParticlePipeline();

        void updateUniformBuffers(const Light &light, const Camera &camera, u_int32_t currentImage);
        void recordCommandBuffer(VkCommandBuffer commandBuffer, SwapChainImageResource imageResource, uint32_t currentFrame, Moon &moon, ParticleSystem &particles);
};