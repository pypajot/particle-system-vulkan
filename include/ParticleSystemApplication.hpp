#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>

#define BASE_WIN_HEIGHT 1080
#define BASE_WIN_WIDTH 1920

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

        const std::vector<const char*> validationLayers =
        {
            "VK_LAYER_KHRONOS_validation"
        };

        #ifdef NDEBUG
            const bool enableValidationLayers = false;
        #else
            const bool enableValidationLayers = true;
        #endif

        bool checkValidationLayerSupport();

        void initWindow();
        void createInstance();
        void initVulkan();
        void mainLoop();
        void cleanup();
};