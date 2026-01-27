#pragma once

#include <vector>
#include <string>
#include <vulkan/vulkan.hpp>

#include "Vertex.hpp"

class Moon
{
    private:
        const std::string MODEL_PATH = "models/moon.obj";
        const std::string TEXTURE_PATH = "textures/moon_diffuse.png";

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;

        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;

        VkImage textureImage;
        VkDeviceMemory textureImageMemory;
        VkImageView textureImageView;

    public:
        void loadTexture(VkDevice device, VkPhysicalDeviceMemoryProperties memProperties, VkQueue graphicsQueue, VkCommandPool commandPool);
        void loadModel();
        void createVertexBuffer(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkPhysicalDeviceMemoryProperties memProperties);
        void createIndexBuffer(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkPhysicalDeviceMemoryProperties memProperties);
        void draw(VkCommandBuffer commandBuffer);

        VkImageView getTextureImageView();
        
    };