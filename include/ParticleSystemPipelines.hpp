#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class ParticleSystemApplication;

class ParticleSystemPipelines
{
    private:
        ParticleSystemApplication *parentApp;

        VkPipelineLayout shadowMapModelPipelineLayout;
        VkPipeline shadowMapModelPipeline;
        
        VkPipelineLayout shadowMapParticlePipelineLayout;
        VkPipeline shadowMapParticlePipeline;
        
        VkPipelineLayout graphicsPipelineLayout;
        VkPipeline graphicsPipeline;

        VkPipelineLayout particleGraphicsPipelineLayout;
        VkPipeline particleGraphicsPipeline;

        VkPipelineLayout particleInitPipelineLayout;
        VkPipeline particleInitPipeline;

        VkPipelineLayout particleUpdatePipelineLayout;
        VkPipeline particleUpdatePipeline;

    public:
        ParticleSystemPipelines();
        ParticleSystemPipelines(const ParticleSystemPipelines &other) = delete;
        ParticleSystemPipelines &operator=(const ParticleSystemPipelines &other) = delete;
        ~ParticleSystemPipelines();

        void bindDevice(VkDevice appDevice);
        VkShaderModule createShaderModule(const std::vector<char>& code);

        void createShadowMapModelPipeline();
        void createShadowMapParticlePipeline();
        void createGraphicsPipeline();
        void createParticleGraphicsPipeline();
        void createParticleInitPipeline();
        void createParticleUpdatePipeline();
        void cleanup();


};