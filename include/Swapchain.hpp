#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>
#include <GLFW/glfw3.h>

struct SwapchainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct SwapChainImageResource
{
    VkImage swapchainImage;
    VkImageView swapchainImageView;
};

class Swapchain
{
    private:
        SwapchainSupportDetails swapChainSupport;

        VkSwapchainKHR swapchain;
        VkFormat swapchainImageFormat;
        VkExtent2D swapchainExtent;

        std::vector<VkImage> swapchainImages;
        std::vector<VkImageView> swapchainImageViews;

        std::vector<VkSemaphore> renderFinishedSemaphore;

    public:
        void create(GLFWwindow *window, VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface);
        void createImageViews(VkDevice device);
        void createSemaphore(VkDevice device);

        VkSemaphore getSemaphore(int imageIndex);
        SwapChainImageResource getImageResource(int imageIndex);
        VkSwapchainKHR getSwapchain();
        VkExtent2D getExtent() const;
        VkFormat getImageFormat() const;
};