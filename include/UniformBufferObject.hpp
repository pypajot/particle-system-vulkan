#pragma once

#include <glm/glm.hpp>

/// @brief Data structure for the model view and projection matrix, for use on the device
struct UniformBufferObject
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 projViewModel;
};