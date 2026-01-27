#pragma once

#include <glm/glm.hpp>

struct Camera
{
    glm::vec3 position = glm::vec3(0.0f, -10.0f, 0.0f);
    glm::vec3 direction = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 up = glm::vec3(-1.0f, 0.0f, 0.0f);
};