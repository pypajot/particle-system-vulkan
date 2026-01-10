#version 450

layout(binding = 0) uniform uniformBufferObject
{
    mat4 model;
    mat4 view;
    mat4 proj;
    mat4 projViewModel;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec2 fragTexCoord;


void main()
{
    gl_Position = ubo.projViewModel * vec4(inPosition, 1.0);
    fragNormal = (ubo.model * vec4(inNormal, 1.0f)).xyz;
    fragTexCoord = inTexCoord;
}