#pragma once

#include <glm/glm.hpp>

/// @brief Data structure containing the light information, for use on the device
struct SceneDataBufferObject
{
    glm::vec4 ambientLight;
    glm::vec4 lightDirection;
    glm::vec4 lightColor;    
};