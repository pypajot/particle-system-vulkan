#pragma once

#include <GLFW/glfw3.h>

#define BASE_WIN_WIDTH 1920
#define BASE_WIN_HEIGHT 1080

class Window
{
    private:
        uint32_t width = BASE_WIN_WIDTH;
        uint32_t height = BASE_WIN_HEIGHT;

        GLFWwindow *window;

    public:
        void init();
        void cleanup();
        
        uint32_t getWidth() const; 
        uint32_t getHeight() const;
        GLFWwindow *getWindow() const;
        bool shouldClose() const;
};