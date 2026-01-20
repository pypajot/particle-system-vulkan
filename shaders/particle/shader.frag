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

    float closestDepth = texture(shadowSampler, coord.xy).r;
    float currentDepth = fragShadowTexCoord.z;

    float shadow = 0.0f;
    float bias = 0.05;
    vec2 texelSize = 1.0 / textureSize(shadowSampler, 0);
    for (int i = -1; i<=1; i++)
    {
        for (int j = -1; j<=1; j++)
        {
            closestDepth = texture(shadowSampler, coord.xy + vec2(i, j) * texelSize).r;
            if (currentDepth - closestDepth > bias)
                shadow += 1.0f;
        }
    }
    shadow /= 9.0;

    return shadow;
}

void main()
{
    // vec3 bump = texture(texBumpSampler, fragTexCoord).rgb;
    // vec3 bumpNorm = fragNormal + bump.xyz;
    // bumpNorm = normalize(bumpNorm);
    // float lightIntensity = dot(scene.sunlightDirection.xyz, fragNormal);
    // if (lightIntensity < 0.0f)
    //     lightIntensity = 0.0f;
    // lightIntensity += scene.ambientLight.x;
    outColor = inColor * ((1 - computeShadow()) + scene.ambientLight);
    // outColor = vec4(closestDepth, currentDepth, 0.0f, 1.0f);
}

