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


#define BASE_WIN_HEIGHT 1080
#define BASE_WIN_WIDTH 1920

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
        bool framebufferResized = false;
        
        ParticleSystemApplication();
        ParticleSystemApplication(const ParticleSystemApplication &other) = delete;
        ParticleSystemApplication &operator=(const ParticleSystemApplication &other) = delete;
        ~ParticleSystemApplication();

        void run();

    private:
        const std::string MODEL_PATH = "models/moon.obj";
        const std::string TEXTURE_PATH = "textures/moon_diffuse.png";
        const std::string BUMP_PATH = "textures/saturn_ring_texture.png";

        const uint kNumberOfFramesInFlight = 2;

        GLFWwindow *window;
        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;
        VkSurfaceKHR surface;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice device;
        VkQueue graphicsAndComputeQueue;
        VkQueue presentQueue;

        VkSwapchainKHR swapChain;

        std::vector<VkImage> swapChainImages;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;
        std::vector<VkImageView> swapChainImageViews;

        // VkRenderPass renderPass;
        VkDescriptorSetLayout shadowMapDescriptorSetLayout;
        
        VkPipelineLayout shadowMapModelPipelineLayout;
        VkPipeline shadowMapModelPipeline;
        
        VkPipelineLayout shadowMapParticlePipelineLayout;
        VkPipeline shadowMapParticlePipeline;
        
        VkDescriptorSetLayout descriptorSetLayout;
        
        VkPipelineLayout graphicsPipelineLayout;
        VkPipeline graphicsPipeline;

        VkPipelineLayout particleGraphicsPipelineLayout;
        VkPipeline particleGraphicsPipeline;

        VkDescriptorSetLayout particleInitDescriptorSetLayout;

        VkPipelineLayout particleInitPipelineLayout;
        VkPipeline particleInitPipeline;

        VkDescriptorSetLayout particleUpdateDescriptorSetLayout;

        VkPipelineLayout particleUpdatePipelineLayout;
        VkPipeline particleUpdatePipeline;

        // std::vector<VkFramebuffer> swapChainFramebuffers;init

        VkCommandPool commandPool;
        std::vector<VkCommandBuffer> commandBuffer;
        std::vector<VkCommandBuffer> computeCommandBuffer;



        std::vector<VkSemaphore> imageAvailableSemaphore;
        std::vector<VkSemaphore> renderFinishedSemaphore;
        std::vector<VkFence> inFlightFence;

        std::vector<VkSemaphore> computeFinishedSemaphore;
        std::vector<VkFence> computeInFlightFences;

        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;

        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;

        std::vector<VkBuffer> shaderStorageBuffers;
        std::vector<VkDeviceMemory> shaderStorageBuffersMemory;

        std::vector<VkBuffer> uniformBuffers;
        std::vector<VkDeviceMemory> uniformBuffersMemory;
        std::vector<void*> uniformBuffersMapped;

        std::vector<VkBuffer> sceneDataBuffers;
        std::vector<VkDeviceMemory> sceneDataBuffersMemory;
        std::vector<void*> sceneDataBuffersMapped;

        std::vector<VkBuffer> shadowMapUniformBuffers;
        std::vector<VkDeviceMemory> shadowMapUniformBuffersMemory;
        std::vector<void*> shadowMapUniformBuffersMapped;

        VkDescriptorPool descriptorPool;
        std::vector<VkDescriptorSet> descriptorSets;
        std::vector<VkDescriptorSet> computeDescriptorSets;
        std::vector<VkDescriptorSet> shadowMapDescriptorSets;

        VkImage colorImage;
        VkDeviceMemory colorImageMemory;
        VkImageView colorImageView;

        VkImage depthImage;
        VkDeviceMemory depthImageMemory;
        VkImageView depthImageView;

        VkImage textureImage;
        VkDeviceMemory textureImageMemory;
        VkImageView textureImageView;

        uint shadowMapHeight = 1024;
        uint shadowMapWidth = 1024;

        VkImage depthMap;
        VkDeviceMemory depthMapMemory;
        VkImageView depthMapView;

        VkSampler textureSampler;
        VkSampler shadowMapTextureSampler;

        VkImage bumpImage;
        VkDeviceMemory bumpImageMemory;
        VkImageView bumpImageView;

        uint32_t currentFrame = 0;

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

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

        void transition_image_layout
        (
            VkCommandBuffer commandBuffer,
            VkImage targetImage,
            VkImageLayout oldLayout,
            VkImageLayout newLayout,
            VkAccessFlags2 srcAccessMask,
            VkAccessFlags2 dstAccessMask,
            VkPipelineStageFlags2 srcStageMask,
            VkPipelineStageFlags2 dstStageMask,
            uint imageAspectFlags
        );

        void createInstance();
        void setupDebugMessenger();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createSurface();
        void createSwapChain();
        void recreateSwapChain();
        void cleanupSwapChain();
        void createImageViews();

        void createShadowMapModelPipeline();
        void createShadowMapParticlePipeline();

        void createGraphicsPipeline();
        void createParticleGraphicsPipeline();
        void createParticleInitPipeline();
        void createParticleUpdatePipeline();
        // void createFramebuffers();
        void createCommandPool();
        void createCommandBuffer();

        void recordShadowMapRendering(VkCommandBuffer commandBuffer);
        void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
        void copyBuffer(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

        void loadModel();

        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        void createVertexBuffer(VkCommandBuffer commandBuffer);
        void createIndexBuffer(VkCommandBuffer commandBuffer);
        void createStorageBuffers();
        void createUniformBuffers();
        void createSceneDataBuffers();
        void createShadowMapUniformBuffers();

        void createDescriptorPool();
        void createDescriptorSets();
        void createDescriptorSetLayout();
        void createComputeDescriptorSetLayout();
        void createShadowMapDescriptorSetLayout();
        void updateUniformBuffer(uint32_t currentImage);

        void createColorResources();
        void createDepthResources();
        VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
        VkFormat findDepthFormat();
        bool hasStencilComponent(VkFormat format);

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        void createImage(uint32_t width, uint32_t height, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
        VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

        void createTextureSampler();
        void createShadowMapTextureSampler();

        void createTextureImage(VkCommandBuffer commandBuffer);
        void createTextureImageView();
        
        void createBumpImage(VkCommandBuffer commandBuffer);
        void createBumpImageView();

        void createDepthMap();
        
        void copyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

        void recordComputeCommandBuffer(VkCommandBuffer commandBuffer, VkPipeline computePipeline, VkPipelineLayout computePipelineLayout);

        void resetParticle();
        void updateParticles();
        void drawFrame();

        void createSyncObjects();

        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer commandBuffer, VkQueue submitQueue);
        void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

        VkShaderModule createShaderModule(const std::vector<char>& code);

        VkSampleCountFlagBits getMaxUsableSampleCount();

        void initVulkan();
        void mainLoop();
        void cleanup();
};