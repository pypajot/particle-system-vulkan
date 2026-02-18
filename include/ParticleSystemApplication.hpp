#pragma once

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>
#include <optional>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "Window.hpp"
#include "Swapchain.hpp"
#include "Pipelines/RenderingPipeline.hpp"
#include "Pipelines/ComputePipeline.hpp"
#include "Pipelines/ShadowmapPipeline.hpp"
#include "Objects/Moon.hpp"
#include "Objects/Rings.hpp"
#include "DataObjects/Vertex.hpp"

#define NUMBER_OF_FRAMES_IN_FLIGHT 2

/// @brief The main class used to manage vulkan initialization and launch the application 
class ParticleSystemApplication
{
    public:
        void run();

    private:
        /// @brief Window object for the app
        Window window;
        
        /// @brief VUlkan instance
        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;
        VkSurfaceKHR surface;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkPhysicalDeviceMemoryProperties memProperties;
        /// @brief Vulkan logical device
        VkDevice device;
        VkSampleCountFlagBits msaaSamples;

        /// @brief Texture sampler used for both the loaded texture and shadowmap
        VkSampler textureSampler;

        VkDescriptorPool descriptorPool;

        Swapchain swapchain;

        /// @brief Graphics and compute queue
        /// @note Graphics and compute are guarateed to have a queue family in common in Vulkan
        VkQueue graphicsAndComputeQueue;
        VkQueue presentQueue;

        Moon moon;
        Rings rings;

        Camera camera;
        Light light;

        RenderingPipeline renderingPipeline;
        ShadowmapPipeline shadowMapPipeline;
        ComputePipeline computePipeline;

        VkCommandPool commandPool;
        std::vector<VkCommandBuffer> commandBuffer;
        std::vector<VkCommandBuffer> computeCommandBuffer;

        std::vector<VkSemaphore> imageAvailableSemaphore;
        std::vector<VkFence> inFlightFence;

        /// @brief Counter for the frames in flight
        uint32_t currentFrame = 0;

        const std::vector<const char*> validationLayers =
        {
            "VK_LAYER_KHRONOS_validation"
        };

        const std::vector<const char*> deviceExtensions =
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
            VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME,
            VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
            VK_KHR_MAINTENANCE_2_EXTENSION_NAME,
            VK_KHR_MULTIVIEW_EXTENSION_NAME

        };

        #ifdef NDEBUG
            const bool enableValidationLayers = false;
        #else
            const bool enableValidationLayers = true;
        #endif

        bool checkValidationLayerSupport();

        std::vector<const char*> getRequiredExtensions();
        bool checkDeviceExtensionSupport(VkPhysicalDevice device);
        int rateDeviceSuitability(VkPhysicalDevice device);
        
        void createInstance();
        void setupDebugMessenger();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createSurface();
        
        void createDescriptorPool();
        void createCommandPool();
        void createCommandBuffer();
        void createTextureSamplers();

        bool hasStencilComponent(VkFormat format);

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
        void drawFrame();

        void createSyncObjects();

        VkSampleCountFlagBits getMaxUsableSampleCount();

        void initVulkan();
        void mainLoop();
        void cleanup();
};