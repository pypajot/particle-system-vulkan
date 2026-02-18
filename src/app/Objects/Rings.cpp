#include "Objects/Rings.hpp"
#include "ParticleSystemApplication.hpp"
#include "ParticleSystemUtils.hpp"

void Rings::loadTexture(VkDevice device, VkPhysicalDeviceMemoryProperties memProperties, VkQueue graphicsQueue, VkCommandPool commandPool)
{
    createTextureImage(RING_TEXTURE_PATH, ringImage, ringImageMemory, device, memProperties, graphicsQueue, commandPool);
}

void Rings::createBuffers(VkDevice device, VkPhysicalDeviceMemoryProperties memProperties)
{
    shaderStorageBuffers.resize(NUMBER_OF_FRAMES_IN_FLIGHT);
    shaderStorageBuffersMemory.resize(NUMBER_OF_FRAMES_IN_FLIGHT);

    VkDeviceSize bufferSize = sizeof(Particle) * PARTICLE_NUMBER;

    for (uint32_t i = 0; i < NUMBER_OF_FRAMES_IN_FLIGHT; i++)
        createBuffer(device, bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, shaderStorageBuffers[i], shaderStorageBuffersMemory[i], memProperties);
    
}

void Rings::draw(VkCommandBuffer commandBuffer, uint32_t currentFrame)
{
    VkBuffer vertexBuffers[] = {shaderStorageBuffers[currentFrame]};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdDraw(commandBuffer, PARTICLE_NUMBER, 1, 0, 0);
}

std::vector<VkBuffer> Rings::getBuffers()
{
    return shaderStorageBuffers;
}
