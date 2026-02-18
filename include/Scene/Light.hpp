#pragma once

#include <glm/glm.hpp>

/// @brief Directional light object in a 3-dimensional space
struct Light
{
    /// @brief RGB Color of the ambient light
    float ambient = 0.1f;
    /// @brief Direction of the light
    glm::vec3 direction = glm::vec3(10.0f, 2.0f, 0.0f);
    /// @brief  RGB Color of the light
    glm::vec3 color = glm::vec3(1.0f);
};