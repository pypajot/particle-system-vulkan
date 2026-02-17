#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>

#include "Particle.hpp"

#define PARTICLE_NUMBER 16777216

/// @brief The particle system object and the related texture
class Rings
{
    private:
        /// @brief Path to the ring texture
        const std::string RING_TEXTURE_PATH = "textures/saturn_ring_texture.png";

        /// @brief Device buffers for the particles
        std::vector<VkBuffer> shaderStorageBuffers;
        /// @brief Device buffer memory for the particles
        std::vector<VkDeviceMemory> shaderStorageBuffersMemory;
        
        /// @brief Image for the ring texture
        VkImage ringImage;
        /// @brief Memory for the ring texture
        VkDeviceMemory ringImageMemory;
        /// @brief Image view for the ring texture
        VkImageView ringImageView;

    public:
        void loadTexture(VkDevice device, VkPhysicalDeviceMemoryProperties memProperties, VkQueue graphicsQueue, VkCommandPool commandPool);
        void createBuffers(VkDevice device, VkPhysicalDeviceMemoryProperties memProperties);
        void draw(VkCommandBuffer commandBuffer, uint32_t currentFrame);

        std::vector<VkBuffer> getBuffers();
    };
