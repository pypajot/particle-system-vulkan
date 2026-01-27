#include "Window.hpp"

#include <stdexcept>

void Window::init()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(width, height, "Vulkan window", nullptr, nullptr);

    if (window == nullptr)
        throw std::runtime_error("Failed to create window.");
}

void Window::cleanup()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

uint32_t Window::getWidth() const
{
    return width;
}

uint32_t Window::getHeight() const
{
    return height;
}

GLFWwindow *Window::getWindow() const
{
    return window;
}

bool Window::shouldClose() const
{
    return glfwWindowShouldClose(window);
}
