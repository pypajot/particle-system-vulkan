#include "ParticleSystemApplication.hpp"

#include <cstring>

ParticleSystemApplication::ParticleSystemApplication()
{
}

ParticleSystemApplication::~ParticleSystemApplication()
{
}

void ParticleSystemApplication::run()
{
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}

void ParticleSystemApplication::initWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(BASE_WIN_WIDTH, BASE_WIN_HEIGHT, "Vulkan window", nullptr, nullptr);

    if (window == nullptr)
        throw std::runtime_error("Failed to create window.");
}

bool ParticleSystemApplication::checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers)
    {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers)
        {
            std::cout << layerProperties.layerName << "\n";
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
// void checkExtensions(std::vector<VkExtensionProperties> availableExtensions, const char** glfwExtensions, int extensionsCount)
// {
//     std::unordered_set<std::string> ext;
//     for (const auto &extension: availableExtensions)
//     {
//         ext.insert(extension.extensionName);
//     }

//     while (extensionsCount)
//     {
//         if (!ext.count(*glfwExtensions))
//         {
//             std::cout << *glfwExtensions << " is missing\n";
//             std::cout << "False\n";
//             return;
//         }
//         glfwExtensions++;
//         extensionsCount--;
//     }
//     std::cout << "True\n";
// }

void ParticleSystemApplication::createInstance()
{
    if (enableValidationLayers && !checkValidationLayerSupport())
        throw std::runtime_error("Validation layers requested but not available.");

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Particle system";
    appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 4, 328);
    appInfo.pEngineName = "Particle engine";
    appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 4, 328);
    appInfo.apiVersion = VK_MAKE_API_VERSION(0, 1, 4, 0);

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    
    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
        createInfo.enabledLayerCount = 0;

    if (vkCreateInstance(&createInfo, nullptr, &instance))
        throw std::runtime_error("Failed to create instance.");
}

void ParticleSystemApplication::initVulkan()
{


    createInstance();
}

void ParticleSystemApplication::mainLoop()
{
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }
}

void ParticleSystemApplication::cleanup()
{
    vkDestroyInstance(instance, nullptr);
    glfwDestroyWindow(window);
    glfwTerminate();
}
