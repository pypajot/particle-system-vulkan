#pragma once

#include <glm/glm.hpp>

/// @brief Camera object in a 3-dimensional space
struct Camera
{
    /// @brief Camera position
    glm::vec3 position = glm::vec3(0.0f, -10.0f, 0.0f);
    /// @brief Camera direction
    glm::vec3 direction = glm::vec3(0.0f, 1.0f, 0.0f);
    /// @brief Camera up vector
    glm::vec3 up = glm::vec3(-1.0f, 0.0f, 0.0f);
};