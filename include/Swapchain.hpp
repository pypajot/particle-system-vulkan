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

/// @brief Class managing the swapchain and its related data
class Swapchain
{
    private:
        // SwapchainSupportDetails swapChainSupport;
        /// @brief The swapcchain object
        VkSwapchainKHR swapchain;
        /// @brief The swapchain image format
        VkFormat swapchainImageFormat;
        /// @brief The swapchain images dimensions
        VkExtent2D swapchainExtent;

        /// @brief The swapchain images
        std::vector<VkImage> swapchainImages;
        /// @brief The swapchain image views
        std::vector<VkImageView> swapchainImageViews;

        /// @brief The semaphores used to signal the image are ready for presentation
        std::vector<VkSemaphore> renderFinishedSemaphore;

    public:
        void create(GLFWwindow *window, VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface);
        void createImageViews(VkDevice device);
        void createSemaphore(VkDevice device);

        /// @brief Get the semaphore associated with an index corresponding to a swapchain image
        VkSemaphore getSemaphore(int imageIndex);
        /// @brief Get the resources associated with an index corresponding to a swapchain image
        SwapChainImageResource getImageResource(int imageIndex);
        VkSwapchainKHR getSwapchain();
        VkExtent2D getExtent() const;
        VkFormat getImageFormat() const;
};