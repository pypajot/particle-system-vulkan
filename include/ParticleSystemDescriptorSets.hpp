#pragma once

#include <vulkan/vulkan.h>
#include <vector>


class ParticleSystemDescriptorSets
{
    private:
        VkDescriptorPool descriptorPool;

        VkDescriptorSetLayout renderDescriptorSetLayout;
        std::vector<VkDescriptorSet> renderDescriptorSets;
        
        VkDescriptorSetLayout computeDescriptorSetLayout;
        std::vector<VkDescriptorSet> computeDescriptorSets;
        
        VkDescriptorSetLayout particleInitDescriptorSetLayout;

        VkDescriptorSetLayout shadowMapDescriptorSetLayout;
        std::vector<VkDescriptorSet> shadowMapDescriptorSets;

    public:
        ParticleSystemDescriptorSets();
        ParticleSystemDescriptorSets(const ParticleSystemDescriptorSets &other) = delete;
        ParticleSystemDescriptorSets &operator=(const ParticleSystemDescriptorSets &other) = delete;
        ~ParticleSystemDescriptorSets();
        
        void createDescriptorPool(VkDevice device);
        void createRenderDescriptorSets(VkDevice device);
        void createComputeDescriptorSets(VkDevice device);
        void createShadowMapDescriptorSets(VkDevice device);
        void createDescriptorSetLayout(VkDevice device);
        void createComputeDescriptorSetLayout(VkDevice device);
        void createShadowMapDescriptorSetLayout(VkDevice device);
};