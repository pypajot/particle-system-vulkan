#pragma once

#include <vulkan/vulkan.hpp>

#include "Moon.hpp"
#include "Rings.hpp"
#include "Light.hpp"

#define SHADOW_MAP_WIDTH 1024
#define SHADOW_MAP_HEIGHT 1024

/// @brief Class that manages the pipelines for the shadow map and its associated objects
/// @note The render target for these pipeline is used as a texture in the rendering pipeline
class ShadowmapPipeline
{
    private:
        /// @brief Copy of the device from the app, here for convenience
        VkDevice device;
        /// @brief Copy of the memory properties from the app, here for convenience
        VkPhysicalDeviceMemoryProperties memProperties;
        /// @brief Copy of the depth format from the app, here for convenience
        VkFormat depthFormat; 

        /// @brief Shadow map image dimensions
        VkExtent2D shadowMapExtent{SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT};

        /// @brief Uniform buffers for the shadow map pipeline
        std::vector<VkBuffer> uniformBuffers;
        /// @brief Device memory for the uniforms buffers
        std::vector<VkDeviceMemory> uniformBuffersMemory;
        /// @brief Host mapped memory for the uniforms buffers
        std::vector<void*> uniformBuffersMapped;

        /// @brief Layout for the descriptor sets
        VkDescriptorSetLayout descriptorSetLayout;
        /// @brief The descriptor sets for the pipeline
        std::vector<VkDescriptorSet> descriptorSets;

        /// @brief Render target for the pipeline
        VkImage depthMap;
        /// @brief Device memory for teh depth map
        VkDeviceMemory depthMapMemory;
        /// @brief Image view for the depth map
        VkImageView depthMapView;

        /// @brief Pipeline layout for the moon object
        VkPipelineLayout modelPipelineLayout;
        /// @brief Pipeline for the moon object
        VkPipeline modelPipeline;
        
        /// @brief Pipeline layout for the particles
        VkPipelineLayout particlePipelineLayout;
        /// @brief Pipeline for the particles
        VkPipeline particlePipeline;

    public:
        /// @brief Initialize the instance with information for the app 
        void init(VkDevice device, VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties memProperties);
        void createDepthMap();
        void createUniformBuffers();
        void createDescriptorSetLayout();
        void createDescriptorSets(VkDescriptorPool descriptorPool);
        void createModelPipeline();
        void createParticlePipeline();

        VkImageView getDepthMapView() const;
        std::vector<VkBuffer> getUniformBuffers() const;
        
        /// @brief Update the uniform buffers with information from the light object and the current frame in flight
        void updateUniformBuffers(const Light &light, u_int32_t currentFrame);
        /// @brief Record the rendering of the shadowmap pipeline
        void recordRendering(VkCommandBuffer commandBuffer, uint32_t currentFrame, Moon &moon, Rings &particles);
};