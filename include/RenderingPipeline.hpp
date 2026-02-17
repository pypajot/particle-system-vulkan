#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>

#include "ShadowmapPipeline.hpp"
#include "Swapchain.hpp"
#include "Light.hpp"
#include "Camera.hpp"

/// @brief Class that manages the pipelines used for rendering to the swapchain images and its associated objects
class RenderingPipeline
{
    private:
        /// @brief Copy of the device from the app, here for convenience
        VkDevice device;
        /// @brief Copy of the memory properties from the app, here for convenience
        VkPhysicalDeviceMemoryProperties memProperties;
        /// @brief Copy of the sample count from the app, here for convenience
        VkSampleCountFlagBits msaaSamples;
        /// @brief Copy of the swapchain dimensions from the app, here for convenience
        VkExtent2D swapChainExtent;
        /// @brief Copy of the swapchain image format from the app, here for convenience
        VkFormat swapChainImageFormat;
        /// @brief Copy of the depth format from the app, here for convenience
        VkFormat depthFormat;

        /// @brief Uniform buffer for the camera and model
        std::vector<VkBuffer> uniformBuffers;
        /// @brief Device memory for the uniform buffers
        std::vector<VkDeviceMemory> uniformBuffersMemory;
        /// @brief Mapped host address for the uniform buffers
        std::vector<void*> uniformBuffersMapped;

        /// @brief Buffers for the light information
        std::vector<VkBuffer> sceneDataBuffers;
        /// @brief Device memory for the light buffers
        std::vector<VkDeviceMemory> sceneDataBuffersMemory;
        /// @brief Mapped host address for the light buffers
        std::vector<void*> sceneDataBuffersMapped;

        /// @brief Layout for the decriptor sets
        VkDescriptorSetLayout descriptorSetLayout;
        /// @brief The descriptor sets used in both rendering pipeline (model and particles)
        std::vector<VkDescriptorSet> descriptorSets;

        /// @brief Render target for the pipelines
        VkImage colorImage;
        /// @brief Device memory for the rendering target
        VkDeviceMemory colorImageMemory;
        /// @brief Image view for the render target
        VkImageView colorImageView;
        
        /// @brief Depth image used for the pipelines
        VkImage depthImage;
        /// @brief Device memory for the depth image
        VkDeviceMemory depthImageMemory;
        /// @brief Image view for the depth image
        VkImageView depthImageView;

        /// @brief Layout for the model pipeline
        VkPipelineLayout modelPipelineLayout;
        /// @brief Pipeline for the model rendering
        VkPipeline modelPipeline;
        
        /// @brief Layout for the particles pipeline
        VkPipelineLayout particlePipelineLayout;
        /// @brief Pipeline for the particles rendering
        VkPipeline particlePipeline;

    public:
        /// @brief Initialize the instance with information for the app 
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

        /// @brief Update the uniform buffers with information from the light object, camera, and the current frame in flight
        void updateUniformBuffers(const Light &light, const Camera &camera, u_int32_t currentImage);
        /// @brief Record the rendering of the rendering pipeline
        void recordRendering(VkCommandBuffer commandBuffer, SwapChainImageResource imageResource, uint32_t currentFrame, Moon &moon, Rings &particles);
};