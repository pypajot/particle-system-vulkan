#include "ParticleSystemApplication.hpp"
#include "ParticleSystemUtils.hpp"
#include "DataObjects/Particle.hpp"

#include <cstring>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <limits>
#include <algorithm>
#include <fstream>

bool ParticleSystemApplication::checkValidationLayerSupport()
{
    uint32_t layerCount;
    VK_CHECK(vkEnumerateInstanceLayerProperties(&layerCount, nullptr));

    std::vector<VkLayerProperties> availableLayers(layerCount);
    VK_CHECK(vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data()));

    for (const char* layerName : validationLayers)
    {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
            return false;
    }
    return true;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback
(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData
)
{
    (void)messageType;
    (void)pUserData;

    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

std::vector<const char*> ParticleSystemApplication::getRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers)
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

    return extensions;
}

void ParticleSystemApplication::createInstance()
{
    if (enableValidationLayers && !checkValidationLayerSupport())
        throw std::runtime_error("Validation layers requested but not available.");

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Particle system";
    appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 3, 0);
    appInfo.pEngineName = "Particle engine";
    appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 3, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    
    std::vector<const char*> extensions = getRequiredExtensions();
    
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    }
    else 
    {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
        createInfo.enabledLayerCount = 0;

    VK_CHECK(vkCreateInstance(&createInfo, nullptr, &instance));
}

VkResult CreateDebugUtilsMessengerEXT
(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger
)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    else
        return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void ParticleSystemApplication::setupDebugMessenger()
{
    if (!enableValidationLayers)
        return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    VK_CHECK(CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger));
}

bool ParticleSystemApplication::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount;
    VK_CHECK(vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr));

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    VK_CHECK(vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data()));

    std::unordered_set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions)
        requiredExtensions.erase(extension.extensionName);

    return requiredExtensions.empty();
}

VkSampleCountFlagBits ParticleSystemApplication::getMaxUsableSampleCount()
{
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

    VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
    if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
    if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
    if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
    if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
    if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
    if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

    return VK_SAMPLE_COUNT_1_BIT;
}

int ParticleSystemApplication::rateDeviceSuitability(VkPhysicalDevice device)
{
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    int score = 0;

    QueueFamilyIndices indices = findQueueFamilies(device, surface);
    if (!indices.isComplete())
        return 0;

    bool swapChainAdequate = false;
    bool extensionsSupported = checkDeviceExtensionSupport(device);

    if (extensionsSupported)
    {
        SwapchainSupportDetails swapChainSupport = querySwapChainSupport(device, surface);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    if (!swapChainAdequate)
        return 0;

    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        score += 1000;

    score += deviceProperties.limits.maxImageDimension2D;

    if (!deviceFeatures.geometryShader)
        return 0;

    if (!deviceFeatures.samplerAnisotropy)
        return 0;

    return score;
}

void ParticleSystemApplication::pickPhysicalDevice()
{
    uint32_t deviceCount = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr));

    if (deviceCount == 0)
        throw std::runtime_error("Failed to find GPUs with Vulkan support.");

    std::vector<VkPhysicalDevice> devices(deviceCount);
    VK_CHECK(vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data()));

    std::multimap<int, VkPhysicalDevice> candidates;

    for (const auto& device : devices)
    {
        int score = rateDeviceSuitability(device);
        candidates.insert(std::make_pair(score, device));
    }

    if (candidates.rbegin()->first > 0)
    {
        physicalDevice = candidates.rbegin()->second;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
        msaaSamples = getMaxUsableSampleCount();
    }
    else
        throw std::runtime_error("Failed to find a suitable GPU.");
}

void ParticleSystemApplication::createLogicalDevice()
{
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    std::unordered_set<uint32_t> uniqueQueueFamilies =
    {
        indices.graphicsAndComputeFamily.value(),
        indices.presentFamily.value()
    };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily: uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }
    
    VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeature{};

    dynamicRenderingFeature.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
    dynamicRenderingFeature.dynamicRendering = VK_TRUE;

    VkPhysicalDeviceSynchronization2Features synchronisation2Feature{};

    synchronisation2Feature.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR;
    synchronisation2Feature.pNext = &dynamicRenderingFeature;
    synchronisation2Feature.synchronization2 = true;

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo{};

    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = &synchronisation2Feature;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
        createInfo.enabledLayerCount = 0;

    VK_CHECK(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device));

    vkGetDeviceQueue(device, indices.graphicsAndComputeFamily.value(), 0, &graphicsAndComputeQueue);
    vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

void ParticleSystemApplication::createSurface()
{
    VK_CHECK(glfwCreateWindowSurface(instance, window.getWindow(), nullptr, &surface));
}

void ParticleSystemApplication::createCommandPool()
{
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice, surface);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsAndComputeFamily.value();

    VK_CHECK(vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool));
}

void ParticleSystemApplication::createCommandBuffer()
{
    commandBuffer.resize(NUMBER_OF_FRAMES_IN_FLIGHT);
    computeCommandBuffer.resize(NUMBER_OF_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = NUMBER_OF_FRAMES_IN_FLIGHT;

    VK_CHECK(vkAllocateCommandBuffers(device, &allocInfo, commandBuffer.data()));

    VK_CHECK(vkAllocateCommandBuffers(device, &allocInfo, computeCommandBuffer.data()));

}

void ParticleSystemApplication::createSyncObjects()
{
    imageAvailableSemaphore.resize(NUMBER_OF_FRAMES_IN_FLIGHT);
    inFlightFence.resize(NUMBER_OF_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (uint i = 0; i < NUMBER_OF_FRAMES_IN_FLIGHT; i++)
    {
        VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphore[i]));
        VK_CHECK(vkCreateFence(device, &fenceInfo, nullptr, &inFlightFence[i]));
    }
}

uint32_t ParticleSystemApplication::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;
    }

    throw std::runtime_error("Failed to find suitable memory type.");
}

void ParticleSystemApplication::createDescriptorPool()
{
    std::array<VkDescriptorPoolSize, 3> descriptorPoolSizes;
    descriptorPoolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorPoolSizes[0].descriptorCount = NUMBER_OF_FRAMES_IN_FLIGHT * 4;

    descriptorPoolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorPoolSizes[1].descriptorCount = NUMBER_OF_FRAMES_IN_FLIGHT * 3;

    descriptorPoolSizes[2].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorPoolSizes[2].descriptorCount = NUMBER_OF_FRAMES_IN_FLIGHT * 2;

    VkDescriptorPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.poolSizeCount = descriptorPoolSizes.size();
    createInfo.pPoolSizes = descriptorPoolSizes.data();
    createInfo.maxSets = NUMBER_OF_FRAMES_IN_FLIGHT * 9;

    VK_CHECK(vkCreateDescriptorPool(device, &createInfo, nullptr, &descriptorPool));
}

void ParticleSystemApplication::createTextureSamplers()
{
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);

    VkSamplerCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    createInfo.magFilter = VK_FILTER_LINEAR;
    createInfo.minFilter = VK_FILTER_LINEAR;
    createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.anisotropyEnable = VK_TRUE;
    createInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    createInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    createInfo.unnormalizedCoordinates = VK_FALSE;
    createInfo.compareEnable = VK_FALSE;
    createInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    VK_CHECK(vkCreateSampler(device, &createInfo, nullptr, &textureSampler));
}

void ParticleSystemApplication::initVulkan()
{
    createInstance();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    swapchain.create(window.getWindow(), physicalDevice, device, surface);
    swapchain.createImageViews(device);
    swapchain.createSemaphore(device);

    createCommandPool();
    createCommandBuffer();
    createSyncObjects();
    createDescriptorPool();
    createTextureSamplers();

    moon.load(device, memProperties, graphicsAndComputeQueue, commandPool);

    rings.loadTexture(device, memProperties, graphicsAndComputeQueue, commandPool);
    rings.createBuffers(device, memProperties);

    computePipeline.init(device, graphicsAndComputeQueue);
    computePipeline.createCommandBuffers(commandPool);
    computePipeline.createSyncObjects();
    computePipeline.createComputeDescriptorSetLayout();
    computePipeline.createComputeDescriptorSets(rings.getBuffers(), descriptorPool, textureSampler, moon.getTextureImageView());
    computePipeline.createParticleInitPipeline();
    computePipeline.createParticleUpdatePipeline();

    shadowMapPipeline.init(device, physicalDevice, memProperties);
    shadowMapPipeline.createDepthMap();
    shadowMapPipeline.createUniformBuffers();
    shadowMapPipeline.createDescriptorSetLayout();
    shadowMapPipeline.createDescriptorSets(descriptorPool);
    shadowMapPipeline.createModelPipeline();
    shadowMapPipeline.createParticlePipeline();

    renderingPipeline.init(device, physicalDevice, memProperties, swapchain.getImageFormat(), swapchain.getExtent(), msaaSamples);
    renderingPipeline.createColorImage();
    renderingPipeline.createDepthImage();
    renderingPipeline.createUniformBuffers();
    renderingPipeline.createSceneDataBuffers();
    renderingPipeline.createDescriptorSetLayout();
    renderingPipeline.createDescriptorSets(shadowMapPipeline.getUniformBuffers(), moon.getTextureImageView(), textureSampler, shadowMapPipeline.getDepthMapView(), textureSampler, descriptorPool);
    renderingPipeline.createModelPipeline();
    renderingPipeline.createParticlePipeline();
}

bool ParticleSystemApplication::hasStencilComponent(VkFormat format)
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void ParticleSystemApplication::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));
    shadowMapPipeline.recordRendering(commandBuffer, currentFrame, moon, rings);
    renderingPipeline.recordRendering(commandBuffer, swapchain.getImageResource(imageIndex), currentFrame, moon, rings);
    VK_CHECK(vkEndCommandBuffer(commandBuffer));
}

void ParticleSystemApplication::drawFrame()
{
    VK_CHECK(vkWaitForFences(device, 1, &inFlightFence[currentFrame], VK_TRUE, UINT64_MAX));
    VK_CHECK(vkResetFences(device, 1, &inFlightFence[currentFrame]));

    uint32_t imageIndex;
    VK_CHECK(vkAcquireNextImageKHR(device, swapchain.getSwapchain(), UINT64_MAX, imageAvailableSemaphore[currentFrame], VK_NULL_HANDLE, &imageIndex));

    renderingPipeline.updateUniformBuffers(light, camera, currentFrame);
    shadowMapPipeline.updateUniformBuffers(light, currentFrame);

    VK_CHECK(vkResetCommandBuffer(commandBuffer[currentFrame], 0));
    recordCommandBuffer(commandBuffer[currentFrame], imageIndex);

    VkSemaphore computeFinishedSemaphore = computePipeline.getSemaphore(currentFrame);

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphore[currentFrame], computeFinishedSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  
    VkSubmitInfo submitInfo = {};

    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 2;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer[currentFrame];

    VkSemaphore signalSemaphores[] = {swapchain.getSemaphore(imageIndex)};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    VK_CHECK(vkQueueSubmit(graphicsAndComputeQueue, 1, &submitInfo, inFlightFence[currentFrame]));

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapchain.getSwapchain()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    presentInfo.pResults = nullptr;
    VK_CHECK(vkQueuePresentKHR(presentQueue, &presentInfo));
}

void ParticleSystemApplication::mainLoop()
{
    while (!window.shouldClose())
    {
        glfwPollEvents();
        computePipeline.updateParticle(currentFrame);
        drawFrame();
        currentFrame = (currentFrame + 1) % NUMBER_OF_FRAMES_IN_FLIGHT; 
    }
    VK_CHECK(vkDeviceWaitIdle(device));

}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

    if (func != nullptr)
        func(instance, debugMessenger, pAllocator);

}

void ParticleSystemApplication::cleanup()
{
    for (uint i = 0; i < NUMBER_OF_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(device, imageAvailableSemaphore[i], nullptr);
        vkDestroyFence(device, inFlightFence[i], nullptr);
    }
    
    vkDestroyCommandPool(device, commandPool, nullptr);

    vkDestroyDevice(device, nullptr);
    
    if (enableValidationLayers)
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);

    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
    window.cleanup();
}


void ParticleSystemApplication::run()
{
    window.init();
    initVulkan();
    mainLoop();
    cleanup();
}