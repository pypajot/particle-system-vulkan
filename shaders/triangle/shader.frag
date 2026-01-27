#version 450

layout(binding = 2) uniform sceneData
{
    vec4 ambientLight;
    vec4 sunlightDirection;
    vec4 sunlightColor;
} scene;

layout(binding = 3) uniform sampler2D texSampler;
layout(binding = 4) uniform sampler2D shadowSampler;

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec4 fragShadowTexCoord;

layout(location = 0) out vec4 outColor;


float computeShadow(float angle)
{
    vec3 coord = fragShadowTexCoord.xyz / fragShadowTexCoord.w;
    coord = coord * 0.5 + 0.5;

    float closestDepth;
    float currentDepth = fragShadowTexCoord.z; 

    float shadow = 0.0;

    float bias = max(0.005, 0.05 * (1 - angle));
    vec2 texelSize = 1.0 / textureSize(shadowSampler, 0);

    for (float i = -1.0; i <= 1.0; i++)
    {
        for (float j = -1.0; j <= 1.0; j++)
        {
            closestDepth = texture(shadowSampler, coord.xy + vec2(i, j) * texelSize).r;
            if (currentDepth - closestDepth > bias)
                shadow += 1.0;
        }
    }
    shadow /= 9.0;

    return shadow;
}

void main()
{
    float angle = dot(normalize(scene.sunlightDirection).xyz, fragNormal);
    if (angle < 0.0f)
        angle = 0.0f;
    float diffuse = angle;
    float shadow = computeShadow(angle);
    float lightIntensity = angle * (1 - shadow) + scene.ambientLight.x;
    outColor = texture(texSampler, fragTexCoord) * scene.sunlightColor * lightIntensity;
}

