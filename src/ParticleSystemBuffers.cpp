#include <vector>
#include <tiny_obj_loader.h>
#include <glm/gtc/matrix_transform.hpp>

#include "ParticleSystemUtils.hpp"
#include "ParticleSystemApplication.hpp"
#include "ParticleSystemBuffers.hpp"
#include "Particle.hpp"

ParticleSystemBuffers::ParticleSystemBuffers() {}

ParticleSystemBuffers::ParticleSystemBuffers(ParticleSystemApplication *app)
{
    parentApp = app;
}

ParticleSystemBuffers::~ParticleSystemBuffers() {}

void ParticleSystemBuffers::loadModel()
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

            vertex.pos =
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

void ParticleSystemBuffers::copyBuffer(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(device, commandPool);

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(device, commandBuffer, graphicsQueue, commandPool);
}

void ParticleSystemBuffers::createVertexBuffer(VkCommandBuffer commandBuffer)
{
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    VK_CHECK(vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data));
    memcpy(data, vertices.data(), (size_t) bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);
    copyBuffer(commandBuffer, stagingBuffer, vertexBuffer, bufferSize);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void ParticleSystemBuffers::createStorageBuffers()
{
    shaderStorageBuffers.resize(NUMBER_OF_FRAMES_IN_FLIGHT);
    shaderStorageBuffersMemory.resize(NUMBER_OF_FRAMES_IN_FLIGHT);

    VkDeviceSize bufferSize = sizeof(Particle) * PARTICLE_NUMBER;

    for (uint32_t i = 0; i < NUMBER_OF_FRAMES_IN_FLIGHT; i++)
        createBuffer(bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, shaderStorageBuffers[i], shaderStorageBuffersMemory[i]);
    
}

void ParticleSystemBuffers::createIndexBuffer(VkCommandBuffer commandBuffer)
{
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    VK_CHECK(vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data));
    memcpy(data, indices.data(), (size_t) bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

    copyBuffer(commandBuffer, stagingBuffer, indexBuffer, bufferSize);

    // endSingleTimeCommands(commandBuffer, graphicsQueue);
    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void ParticleSystemBuffers::createUniformBuffers()
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    uniformBuffers.resize(NUMBER_OF_FRAMES_IN_FLIGHT);
    uniformBuffersMemory.resize(NUMBER_OF_FRAMES_IN_FLIGHT);
    uniformBuffersMapped.resize(NUMBER_OF_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < NUMBER_OF_FRAMES_IN_FLIGHT; i++)
    {
        createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
        VK_CHECK(vkMapMemory(device, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]));
    }
}

void ParticleSystemBuffers::createSceneDataBuffers()
{
    VkDeviceSize bufferSize = sizeof(SceneData);

    sceneDataBuffers.resize(NUMBER_OF_FRAMES_IN_FLIGHT);
    sceneDataBuffersMemory.resize(NUMBER_OF_FRAMES_IN_FLIGHT);
    sceneDataBuffersMapped.resize(NUMBER_OF_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < NUMBER_OF_FRAMES_IN_FLIGHT; i++)
    {
        createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT, sceneDataBuffers[i], sceneDataBuffersMemory[i]);
        VK_CHECK(vkMapMemory(device, sceneDataBuffersMemory[i], 0, bufferSize, 0, &sceneDataBuffersMapped[i]));
    }
}

void ParticleSystemBuffers::createShadowMapUniformBuffers()
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    shadowMapUniformBuffers.resize(NUMBER_OF_FRAMES_IN_FLIGHT);
    shadowMapUniformBuffersMemory.resize(NUMBER_OF_FRAMES_IN_FLIGHT);
    shadowMapUniformBuffersMapped.resize(NUMBER_OF_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < NUMBER_OF_FRAMES_IN_FLIGHT; i++)
    {
        createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT, shadowMapUniformBuffers[i], shadowMapUniformBuffersMemory[i]);
        VK_CHECK(vkMapMemory(device, shadowMapUniformBuffersMemory[i], 0, bufferSize, 0, &shadowMapUniformBuffersMapped[i]));
    }
}

void ParticleSystemBuffers::updateUniformBuffer(uint32_t currentImage)
{
    float time = glfwGetTime() / 4;
    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0, 1, 0));
    ubo.model = glm::mat4(1.0f);
    ubo.view = glm::lookAt(glm::vec3(5.0f, -10.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), swapChainExtent.width / (float) swapChainExtent.height, 0.1f, 100.0f);
    // ubo.view = glm::lookAt(glm::vec3(10.0f, -2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    // ubo.proj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -20.0f, 20.0f);
    ubo.proj[1][1] *= -1;
    ubo.projViewModel = ubo.proj * ubo.view * ubo.model;

    memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));

    SceneData scene{};
    scene.ambientLight = glm::vec4(0.1f);
    scene.sunlightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    scene.sunlightDirection = glm::vec4(10.0f, 2.0f, 0.0f, 1.0f);

    memcpy(sceneDataBuffersMapped[currentImage], &scene, sizeof(scene));

    ubo.model = glm::mat4(1.0f);
    ubo.view = glm::lookAt(glm::vec3(10.0f, 2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    ubo.proj = glm::ortho(-7.0f, 7.0f, 7.0f, -7.0f, 0.0f, 20.0f);
    // ubo.proj[1][1] *= -1;
    
    ubo.projViewModel = ubo.proj * ubo.view * ubo.model;

    memcpy(shadowMapUniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

void ParticleSystemBuffers::cleanup()
{
    vkDestroyBuffer(device, vertexBuffer, nullptr);
    vkFreeMemory(device, vertexBufferMemory, nullptr);

    vkDestroyBuffer(device, indexBuffer, nullptr);
    vkFreeMemory(device, indexBufferMemory, nullptr);

    for (uint i = 0; i < NUMBER_OF_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroyBuffer(device, shaderStorageBuffers[i], nullptr);
        vkFreeMemory(device, shaderStorageBuffersMemory[i], nullptr);

        vkDestroyBuffer(device, uniformBuffers[i], nullptr);
        vkFreeMemory(device, uniformBuffersMemory[i], nullptr);

        vkDestroyBuffer(device, sceneDataBuffers[i], nullptr);
        vkFreeMemory(device, sceneDataBuffersMemory[i], nullptr);

        vkDestroyBuffer(device, shadowMapUniformBuffers[i], nullptr);
        vkFreeMemory(device, shadowMapUniformBuffersMemory[i], nullptr);
    }
}