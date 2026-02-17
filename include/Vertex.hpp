#pragma once

#include <vulkan/vulkan.h>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <array>

#ifndef GLM_ENABLE_EXPERIMENTAL
    #define GLM_ENABLE_EXPERIMENTAL
    #include <glm/gtx/hash.hpp>
#endif

/// @brief Vertex object in 3-dimensional space
/// @note Needs to be used with triangles
struct Vertex
{
    /// @brief Vertex position
    glm::vec3 position;
    /// @brief Normal vector for the triangle
    glm::vec3 normal;
    /// @brief 2D texture coordinate
    glm::vec2 texCoord;

    static VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
        
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, position);
        
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, normal);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        return attributeDescriptions;
    }

    bool operator==(const Vertex& other) const
    {
        return position == other.position && normal == other.normal && texCoord == other.texCoord;
    }
};


/// @brief Specialization of std::hash for the Vertex class
template<>
struct std::hash<Vertex>
{
    size_t operator()(Vertex const& vertex) const
    {
        return ((std::hash<glm::vec3>()(vertex.position) ^
                (std::hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^
                (std::hash<glm::vec2>()(vertex.texCoord) << 1);
    }
};
