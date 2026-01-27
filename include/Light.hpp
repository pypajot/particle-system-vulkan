#pragma once

#include <glm/glm.hpp>

struct Light
{
    float ambient = 0.1f;
    glm::vec4 color = glm::vec4(1.0f);
    glm::vec3 direction = glm::vec3(10.0f, 2.0f, 0.0f);
};