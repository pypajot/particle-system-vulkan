#version 450

layout (binding = 0) uniform uniformBufferObject
{
    mat4 model;
    mat4 view;
    mat4 proj;
    mat4 projViewModel;
} ubo;

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inVelocity;
layout(location = 2) in vec3 inColor;


void main()
{
    gl_Position = ubo.projViewModel * vec4(inPos, 1.0f);
    gl_PointSize = 1.0f;
}
