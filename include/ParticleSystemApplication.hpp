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
#include "ParticleSystemDescriptorSets.hpp"
#include "ParticleSystemPipelines.hpp"
#include "ParticleSystemImages.hpp"
#include "ParticleSystemBuffers.hpp"

#define NUMBER_OF_FRAMES_IN_FLIGHT 2

#define BASE_WIN_HEIGHT 1080
#define BASE_WIN_WIDTH 1920

#define SHADOW_MAP_HEIGHT 1024
#define SHADOW_MAP_WIDTH 1024

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

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsAndComputeFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete()
    {
        return graphicsAndComputeFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
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
        GLFWwindow *window;
        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;
        VkSurfaceKHR surface;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkPhysicalDeviceMemoryProperties memProperties;
        VkDevice device;
        VkQueue graphicsAndComputeQueue;
        VkQueue presentQueue;

        VkSwapchainKHR swapChain;

        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;
        std::vector<VkImage> swapChainImages;
        std::vector<VkImageView> swapChainImageViews;

        VkExtent2D shadowMapExtent;

        ParticleSystemDescriptorSets descriptorSets;
        ParticleSystemPipelines pipelines;
        ParticleSystemImages images;
        ParticleSystemBuffers buffers;

        VkCommandPool commandPool;
        std::vector<VkCommandBuffer> commandBuffer;
        std::vector<VkCommandBuffer> computeCommandBuffer;

        std::vector<VkSemaphore> imageAvailableSemaphore;
        std::vector<VkSemaphore> renderFinishedSemaphore;
        std::vector<VkFence> inFlightFence;

        std::vector<VkSemaphore> computeFinishedSemaphore;
        std::vector<VkFence> computeInFlightFences;

        uint32_t currentFrame = 0;

        bool particleNeedReset = true;

        VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

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

        void initWindow();

        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

        void createInstance();
        void setupDebugMessenger();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createSurface();
        void createSwapChain();
        void createSwapChainImageViews();

        void createCommandPool();
        void createCommandBuffer();

        void recordShadowMapRendering(VkCommandBuffer commandBuffer);
        void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

        bool hasStencilComponent(VkFormat format);

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        void recordComputeCommandBuffer(VkCommandBuffer commandBuffer, VkPipeline computePipeline, VkPipelineLayout computePipelineLayout);

        void resetParticle();
        void updateParticles();
        void drawFrame();

        void createSyncObjects();

        VkSampleCountFlagBits getMaxUsableSampleCount();

        void initVulkan();
        void mainLoop();
        void cleanup();
};