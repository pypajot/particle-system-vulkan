#include "Objects/Moon.hpp"
#include "DataObjects/Vertex.hpp"
#include "ParticleSystemUtils.hpp"

#ifndef TINYOBJLOADER_IMPLEMENTATION
    #define TINYOBJLOADER_IMPLEMENTATION
#endif

#include <tiny_obj_loader.h>

#include <iostream>

void Moon::loadTexture
(
    VkDevice device,
    VkPhysicalDeviceMemoryProperties memProperties,
    VkQueue graphicsQueue,
    VkCommandPool commandPool
)
{
    createTextureImage(TEXTURE_PATH, textureImage, textureImageMemory, device, memProperties, graphicsQueue, commandPool);
    textureImageView = createImageView(device, textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
}

void Moon::loadModel()
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn;
    std::string err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str()))
        throw std::runtime_error(err);

    std::unordered_map<Vertex, uint32_t> uniqueVertices;

    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            Vertex vertex{};

            vertex.position =
            {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            vertex.texCoord =
            {
                attrib.texcoords[2 * index.texcoord_index + 0],
                attrib.texcoords[2 * index.texcoord_index + 1]
            };

            vertex.normal =
            {
                attrib.normals[3 * index.normal_index + 0],
                attrib.normals[3 * index.normal_index + 1],
                attrib.normals[3 * index.normal_index + 2]
            };

            if (!uniqueVertices.count(vertex))
            {
                vertices.push_back(vertex);
                uniqueVertices[vertex] = uniqueVertices.size();
            }
            indices.push_back(uniqueVertices[vertex]);
        }
    }
}

void Moon::createVertexBuffer(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkPhysicalDeviceMemoryProperties memProperties)
{
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT, stagingBuffer, stagingBufferMemory, memProperties);

    void* data;
    VK_CHECK(vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data));
    memcpy(data, vertices.data(), (size_t) bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);
    createBuffer(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory, memProperties);

    copyBuffer(stagingBuffer, vertexBuffer, bufferSize, device, commandPool, graphicsQueue);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void Moon::createIndexBuffer(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkPhysicalDeviceMemoryProperties memProperties)
{
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT, stagingBuffer, stagingBufferMemory, memProperties);

    void* data;
    VK_CHECK(vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data));
    memcpy(data, indices.data(), (size_t) bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);
    createBuffer(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory, memProperties);

    copyBuffer(stagingBuffer, indexBuffer, bufferSize, device, commandPool, graphicsQueue);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void Moon::load(VkDevice device, VkPhysicalDeviceMemoryProperties memProperties, VkQueue graphicsQueue, VkCommandPool commandPool)
{
    loadModel();
    loadTexture(device, memProperties, graphicsQueue, commandPool);
    createIndexBuffer(device, commandPool, graphicsQueue, memProperties);
    createVertexBuffer(device, commandPool, graphicsQueue, memProperties);
}

void Moon::draw(VkCommandBuffer commandBuffer)
{
    VkBuffer vertexBuffers[] = {vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
}

VkImageView Moon::getTextureImageView()
{
    return textureImageView;
}
