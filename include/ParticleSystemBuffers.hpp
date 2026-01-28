#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>

#include "Vertex.hpp"

class ParticleSystemBuffers
{
    public:
        ParticleSystemBuffers();
        ParticleSystemBuffers(const ParticleSystemBuffers &other) = delete;
        ParticleSystemBuffers &operator=(const ParticleSystemBuffers &other) = delete;
        ~ParticleSystemBuffers();

        void appInfo(VkDevice appDevice, VkPhysicalDeviceMemoryProperties appMemProperties, VkCommandPool appCommandPool, VkQueue appGraphicsQueue);
        void loadModel();

        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        void copyBuffer(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        void createVertexBuffer(VkCommandBuffer commandBuffer);
        void createStorageBuffers();
        void createIndexBuffer(VkCommandBuffer commandBuffer);
        void createUniformBuffers();
        void createSceneDataBuffers();
        void createShadowMapUniformBuffers();

    private:
        const std::string MODEL_PATH = "models/moon.obj";

        VkDevice device;
        VkPhysicalDeviceMemoryProperties memProperties;
        VkCommandPool commandPool;
        VkQueue graphicsQueue;

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;

        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;

        std::vector<VkBuffer> shaderStorageBuffers;
        std::vector<VkDeviceMemory> shaderStorageBuffersMemory;

        std::vector<VkBuffer> uniformBuffers;
        std::vector<VkDeviceMemory> uniformBuffersMemory;
        std::vector<void*> uniformBuffersMapped;

        std::vector<VkBuffer> sceneDataBuffers;
        std::vector<VkDeviceMemory> sceneDataBuffersMemory;
        std::vector<void*> sceneDataBuffersMapped;

        std::vector<VkBuffer> shadowMapUniformBuffers;
        std::vector<VkDeviceMemory> shadowMapUniformBuffersMemory;
        std::vector<void*> shadowMapUniformBuffersMapped;
};