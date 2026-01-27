#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>

#include "Particle.hpp"
#include "ParticleSystemUtils.hpp"

class ComputePipeline
{
    private:
        const std::string RING_TEXTURE_PATH = "textures/saturn_ring_texture.png";

        bool particleNeedReset = true;

        VkDevice device;
        VkQueue queue;

        std::vector<VkCommandBuffer> computeCommandBuffer;

        std::vector<VkFence> computeInFlightFences;
        std::vector<VkSemaphore> computeFinishedSemaphore;

        VkDescriptorSetLayout descriptorSetLayout;
        std::vector<VkDescriptorSet> descriptorSets;

        VkPipelineLayout initPipelineLayout;
        VkPipeline initPipeline;

        VkPipelineLayout updatePipelineLayout;
        VkPipeline updatePipeline;

        void recordComputeCommandBuffer(VkCommandBuffer commandBuffer, VkPipeline computePipeline, VkPipelineLayout computePipelineLayout, int currentFrame);
        void createComputePipeline(VkPipeline &particlePipeline, VkPipelineLayout &particlePipelineLayout, const char *shaderPath);

    public:
        void init(VkDevice device, VkQueue computeQueue);

        void createCommandBuffers(VkCommandPool commandPool);
        void createSyncObjects();
        
        void createComputeDescriptorSetLayout();
        void createComputeDescriptorSets(std::vector<VkBuffer> shaderStorageBuffers, VkDescriptorPool descriptorPool, VkSampler textureSampler, VkImageView ringImageView);

        void createParticleInitPipeline();
        void createParticleUpdatePipeline();

        VkSemaphore getSemaphore(int currentFrame);

        void updateParticle(int currentFrame);

        void cleanup();

};