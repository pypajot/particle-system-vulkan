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

static const std::unordered_map<VkResult, std::string> VulkanErrorToString
{
    {VK_SUCCESS, "Command successfully completed"},
    {VK_NOT_READY, "A fence or query has not yet completed"},
    {VK_TIMEOUT, "A wait operation has not completed in the specified time"},
    {VK_EVENT_SET, "An event is signaled"},
    {VK_EVENT_RESET, "An event is unsignaled"},
    {VK_INCOMPLETE, "A return array was too small for the result"},
    {VK_SUBOPTIMAL_KHR, "A swapchain no longer matches the surface properties exactly, but can still be used to present to the surface successfully"},
    {VK_THREAD_IDLE_KHR, "A deferred operation is not complete but there is currently no work for this thread to do at the time of this call"},
    {VK_THREAD_DONE_KHR, "A deferred operation is not complete but there is no work remaining to assign to additional threads"},
    {VK_OPERATION_DEFERRED_KHR, "A deferred operation was requested and at least some of the work was deferred"},
    {VK_OPERATION_NOT_DEFERRED_KHR, "A deferred operation was requested and no operations were deferred"},
    {VK_PIPELINE_COMPILE_REQUIRED, "A requested pipeline creation would have required compilation, but the application requested compilation to not be performed"},
    // {VK_PIPELINE_BINARY_MISSING_KHR, "The application attempted to create a pipeline binary by querying an internal cache, but the internal cache entry did not exist"},
    // {VK_INCOMPATIBLE_SHADER_BINARY_EXT, "The provided binary shader code is not compatible with this device"},
    {VK_ERROR_OUT_OF_HOST_MEMORY, "A host memory allocation has failed"},
    {VK_ERROR_OUT_OF_DEVICE_MEMORY, "A device memory allocation has failed"},
    {VK_ERROR_INITIALIZATION_FAILED, "Initialization of an object could not be completed for implementation-specific reasons"},
    {VK_ERROR_DEVICE_LOST, "The logical or physical device has been lost"},
    {VK_ERROR_MEMORY_MAP_FAILED, "Mapping of a memory object has failed"},
    {VK_ERROR_LAYER_NOT_PRESENT, "A requested layer is not present or could not be loaded"},
    {VK_ERROR_EXTENSION_NOT_PRESENT, "A requested extension is not supported"},
    {VK_ERROR_FEATURE_NOT_PRESENT, "A requested feature is not supported"},
    {VK_ERROR_INCOMPATIBLE_DRIVER, "The requested version of Vulkan is not supported by the driver or is otherwise incompatible for implementation-specific reasons"},
    {VK_ERROR_TOO_MANY_OBJECTS, "Too many objects of the type have already been created"},
    {VK_ERROR_FORMAT_NOT_SUPPORTED, "A requested format is not supported on this device"},
    {VK_ERROR_FRAGMENTED_POOL, "A pool allocation has failed due to fragmentation of the pool’s memory"},
    {VK_ERROR_SURFACE_LOST_KHR, "A surface is no longer available"},
    {VK_ERROR_NATIVE_WINDOW_IN_USE_KHR, "The requested window is already in use by Vulkan or another API in a manner which prevents it from being used again"},
    {VK_ERROR_OUT_OF_DATE_KHR, "A surface has changed in such a way that it is no longer compatible with the swapchain, and further presentation requests using the swapchain will fail"},
    {VK_ERROR_INCOMPATIBLE_DISPLAY_KHR, "The display used by a swapchain does not use the same presentable image layout, or is incompatible in a way that prevents sharing an image"},
    {VK_ERROR_INVALID_SHADER_NV, "One or more shaders failed to compile or link"},
    {VK_ERROR_OUT_OF_POOL_MEMORY, "A pool memory allocation has failed"},
    {VK_ERROR_INVALID_EXTERNAL_HANDLE, "An external handle is not a valid handle of the specified type"},
    {VK_ERROR_FRAGMENTATION, "A descriptor pool creation has failed due to fragmentation"},
    {VK_ERROR_INVALID_DEVICE_ADDRESS_EXT, "A buffer creation failed because the requested address is not available"},
    {VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS, "A buffer creation or memory allocation failed because the requested address is not available A shader group handle assignment failed because the requested shader group handle information is no longer valid"},
    {VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT, "An operation on a swapchain created with VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT failed as it did not have exclusive full-screen access"},
    // {VK_ERROR_VALIDATION_FAILED, "A command failed because invalid usage was detected by the implementation or a validation layer "},
    {VK_ERROR_COMPRESSION_EXHAUSTED_EXT, "An image creation failed because internal resources required for compression are exhausted"},
    {VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR, "The requested VkImageUsageFlags are not supported"},
    {VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR, "The requested video picture layout is not supported"},
    {VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR, "A video profile operation specified via VkVideoProfileInfoKHR::videoCodecOperation is not supported"},
    {VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR, "Format parameters in a requested VkVideoProfileInfoKHR chain are not supported"},
    {VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR, "Codec-specific parameters in a requested VkVideoProfileInfoKHR chain are not supported"},
    {VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR, "The specified video Std header version is not supported"},
    {VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR, "The specified Video Std parameters do not adhere to the syntactic or semantic requirements of the used video compression standard, or values derived from parameters according to the rules defined by the used video compression standard do not adhere to the capabilities of the video compression standard or the implementation"},
    // {VK_ERROR_NOT_PERMITTED, "The driver implementation has denied a request to acquire a priority above the default priority (VK_QUEUE_GLOBAL_PRIORITY_MEDIUM_EXT) because the application does not have sufficient privileges"},
    // {VK_ERROR_NOT_ENOUGH_SPACE_KHR, "The application did not provide enough space to return all the required data"},
    {VK_ERROR_UNKNOWN, "An unknown error has occurred; either the application has provided invalid input, or an implementation failure has occurred"}
};

#define VK_CHECK(result) VulkanCheckError(result, __FILE__, __LINE__)

static void VulkanCheckError(VkResult result, const char *file, int lineNumber)
{
    if (result == VK_SUCCESS)
    return;
    
    std::string line = std::to_string(lineNumber);
    std::string errorString = "Error: " + VulkanErrorToString.at(result) + "\nin file " + file + "\nline: " + line + "\n";
    throw std::runtime_error(errorString);
}

class ParticleSystemApplication
{
    public:
        ParticleSystemApplication();
        ParticleSystemApplication(const ParticleSystemApplication &other) = delete;
        ParticleSystemApplication &operator=(const ParticleSystemApplication &other) = delete;
        ~ParticleSystemApplication();

        void run();

    private:
        const std::string MODEL_PATH = "models/moon.obj";
        const std::string TEXTURE_PATH = "textures/moon_diffuse.png";
        const std::string RING_TEXTURE_PATH = "textures/saturn_ring_texture.png";

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

        VkExtent2D shadowMapExtent;

        ParticleSystemDescriptorSets descriptorSets;

        ParticleSystemPipelines pipelines;

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
        void createImageViews();

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