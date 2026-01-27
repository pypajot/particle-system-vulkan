#pragma once

#include <vulkan/vulkan.hpp>

#include "Moon.hpp"
#include "ParticleSystem.hpp"
#include "Light.hpp"

#define SHADOW_MAP_WIDTH 1024
#define SHADOW_MAP_HEIGHT 1024

class ShadowmapPipeline
{
    private:
        VkDevice device;
        VkPhysicalDeviceMemoryProperties memProperties;
        VkFormat depthFormat; 

        VkExtent2D shadowMapExtent{SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT};

        std::vector<VkBuffer> uniformBuffers;
        std::vector<VkDeviceMemory> uniformBuffersMemory;
        std::vector<void*> uniformBuffersMapped;

        VkDescriptorSetLayout descriptorSetLayout;
        std::vector<VkDescriptorSet> descriptorSets;

        VkImage depthMap;
        VkDeviceMemory depthMapMemory;
        VkImageView depthMapView;

        VkPipelineLayout modelPipelineLayout;
        VkPipeline modelPipeline;
        
        VkPipelineLayout particlePipelineLayout;
        VkPipeline particlePipeline;

    public:
        void init(VkDevice device, VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties memProperties);
        void createDepthMap();
        void createUniformBuffers();
        void createDescriptorSetLayout();
        void createDescriptorSets(VkDescriptorPool descriptorPool);
        void createModelPipeline();
        void createParticlePipeline();

        VkImageView getDepthMapView() const;
        std::vector<VkBuffer> getUniformBuffers() const;
        
        void updateUniformBuffers(const Light &light, u_int32_t currentImage);
        void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t currentFrame, Moon &moon, ParticleSystem &particles);
};