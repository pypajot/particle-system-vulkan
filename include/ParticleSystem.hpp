#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>

#include "Particle.hpp"

class ParticleSystem
{
    private:
        const std::string RING_TEXTURE_PATH = "textures/saturn_ring_texture.png";

        std::vector<VkBuffer> shaderStorageBuffers;
        std::vector<VkDeviceMemory> shaderStorageBuffersMemory;
        
        VkImage ringImage;
        VkDeviceMemory ringImageMemory;
        VkImageView ringImageView;

    public:
        void loadTexture(VkDevice device, VkPhysicalDeviceMemoryProperties memProperties, VkQueue graphicsQueue, VkCommandPool commandPool);
        void createBuffers(VkDevice device, VkPhysicalDeviceMemoryProperties memProperties);
        void draw(VkCommandBuffer commandBuffer, uint32_t currentFrame);

        std::vector<VkBuffer> getBuffers();
    };
