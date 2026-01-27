#pragma once

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>
#include <optional>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "Vertex.hpp"
#include "Window.hpp"
#include "Swapchain.hpp"
#include "RenderingPipeline.hpp"
#include "ComputePipeline.hpp"
#include "ShadowmapPipeline.hpp"
#include "Moon.hpp"
#include "ParticleSystem.hpp"

#define NUMBER_OF_FRAMES_IN_FLIGHT 2

#define PARTICLE_NUMBER 16777216

struct SceneData
{
    glm::vec4 ambientLight;
    glm::vec4 sunlightDirection;
    glm::vec4 sunlightColor;
};

struct UniformBufferObject
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 projViewModel;
};

class ParticleSystemApplication
{
    public:
        ParticleSystemApplication();
        ParticleSystemApplication(const ParticleSystemApplication &other) = delete;
        ParticleSystemApplication &operator=(const ParticleSystemApplication &other) = delete;
        ~ParticleSystemApplication();

        void run();

    private:
        Window window;
        
        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;
        VkSurfaceKHR surface;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkPhysicalDeviceMemoryProperties memProperties;
        VkDevice device;
        VkSampleCountFlagBits msaaSamples;
        VkSampler textureSampler;

        VkDescriptorPool descriptorPool;

        Swapchain swapchain;

        VkQueue graphicsAndComputeQueue;
        VkQueue presentQueue;

        Moon moon;
        ParticleSystem particleSystem;

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

        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
        
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