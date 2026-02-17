#pragma once

#include <vector>
#include <string>
#include <vulkan/vulkan.hpp>

#include "Vertex.hpp"

/// @brief Moon object and its necessary data
class Moon
{
    private:
        /// @brief Path of the model
        const std::string MODEL_PATH = "models/moon.obj";
        /// @brief Path of the model diffuse texture
        const std::string TEXTURE_PATH = "textures/moon_diffuse.png";

        /// @brief Array of the model vertices on the host memory
        std::vector<Vertex> vertices;
        /// @brief Array of the model vertex indices on the host memory
        std::vector<uint32_t> indices;

        /// @brief Device buffer containing the model vertices
        VkBuffer vertexBuffer;
        /// @brief Device memory for the vertex buffer
        VkDeviceMemory vertexBufferMemory;

        /// @brief Device buffer containing the model vertex indices
        VkBuffer indexBuffer;
        /// @brief Device memory for the index buffer
        VkDeviceMemory indexBufferMemory;

        /// @brief Image containing the moon diffuse texture
        VkImage textureImage;
        /// @brief Device memory for the moon diffuse texture
        VkDeviceMemory textureImageMemory;
        /// @brief Image view for the moon diffuse texture
        VkImageView textureImageView;

        void loadModel();
        void loadTexture(VkDevice device, VkPhysicalDeviceMemoryProperties memProperties, VkQueue graphicsQueue, VkCommandPool commandPool);
        void createVertexBuffer(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkPhysicalDeviceMemoryProperties memProperties);
        void createIndexBuffer(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkPhysicalDeviceMemoryProperties memProperties);

    public:
        /// @brief Initialize the moon object
        void load(VkDevice device, VkPhysicalDeviceMemoryProperties memProperties, VkQueue graphicsQueue, VkCommandPool commandPool);

        /// @brief Draw the object during rendering
        /// @param commandBuffer The buffer to which the command will be recorded
        void draw(VkCommandBuffer commandBuffer);

        /// @return The diffuse texture image view
        VkImageView getTextureImageView();
    };