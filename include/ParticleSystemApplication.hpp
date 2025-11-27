#pragma once

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>
#include <optional>
#include <glm/glm.hpp>

#include "Vertex.hpp"


#define BASE_WIN_HEIGHT 1080
#define BASE_WIN_WIDTH 1920

struct UniformBufferObject
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;

    glm::mat4 getTransform()
    {
        return proj * view * model;
    }
};

struct test
{
    glm::mat4 p;
};

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
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
        const uint kNumberOfFramesInFlight = 2;

        GLFWwindow *window;
        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;
        VkSurfaceKHR surface;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice device;
        VkQueue graphicsQueue;
        VkQueue presentQueue;

        VkSwapchainKHR swapChain;

        std::vector<VkImage> swapChainImages;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;
        std::vector<VkImageView> swapChainImageViews;

        VkRenderPass renderPass;
        VkDescriptorSetLayout descriptorSetLayout;
        VkPipelineLayout pipelineLayout;
        
        VkPipeline graphicsPipeline;

        std::vector<VkFramebuffer> swapChainFramebuffers;

        VkCommandPool commandPool;
        std::vector<VkCommandBuffer> commandBuffer;

        std::vector<VkSemaphore> imageAvailableSemaphore;
        std::vector<VkSemaphore> renderFinishedSemaphore;
        std::vector<VkFence> inFlightFence;

        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;

        std::vector<VkBuffer> uniformBuffers;
        std::vector<VkDeviceMemory> uniformBuffersMemory;
        std::vector<void*> uniformBuffersMapped;

        VkDescriptorPool descriptorPool;
        std::vector<VkDescriptorSet> descriptorSets;


        uint32_t currentFrame = 0;
        glm::mat4 proj;

        const std::vector<Vertex> vertices =
        {
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
        };

        const std::vector<const char*> validationLayers =
        {
            "VK_LAYER_KHRONOS_validation"
        };

        const std::vector<const char*> deviceExtensions =
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
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
        bool isDeviceSuitable(VkPhysicalDevice device);

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
        void createImageViews();
        void createRenderPass();
        void createGraphicsPipeline();
        void createFramebuffers();
        void createCommandPool();
        void createCommandBuffer();
        void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        void createVertexBuffer();
        void createUniformBuffers();
        void createDescriptorPool();
        void createDescriptorSets();
        void createDescriptorSetLayout();
        void updateUniformBuffer(uint32_t currentImage);

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        void drawFrame();

        void createSyncObjects();



        VkShaderModule createShaderModule(const std::vector<char>& code);


        void initVulkan();
        void mainLoop();
        void cleanup();
};