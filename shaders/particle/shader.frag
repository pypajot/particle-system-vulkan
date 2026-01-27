#version 450

layout(binding = 2) uniform sceneData
{
    vec4 ambientLight;
    vec4 sunlightDirection;
    vec4 sunlightColor;
} scene;

layout(binding = 4) uniform sampler2D shadowSampler;

layout(location = 0) in vec4 inColor;
layout(location = 1) in vec4 fragShadowTexCoord;

layout(location = 0) out vec4 outColor;

float computeShadow()
{
    vec3 coord = fragShadowTexCoord.xyz / fragShadowTexCoord.w;
    coord = coord * 0.5 + 0.5;

    float closestDepth;
    float currentDepth = fragShadowTexCoord.z;

    float shadow = 0.0;
    float bias = 0.05;
    vec2 texelSize = 1.0 / textureSize(shadowSampler, 0);
    for (float i = -0.5; i <= 0.5; i++)
    {
        for (float j = -0.5; j <= 0.5; j++)
        {
            closestDepth = texture(shadowSampler, coord.xy + vec2(i, j) * texelSize).r;
            if (currentDepth - closestDepth > bias)
                shadow += 1.0;
        }
    }
    shadow /= 4.0;
    return shadow;
}

void main()
{
    outColor = inColor * ((1.0 - computeShadow()) + scene.ambientLight);
}

