#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>

#include "Particle.hpp"
#include "ParticleSystemUtils.hpp"

/// @brief Class that manages objects related to the compute pipeline
class ComputePipeline
{
    private:
        /// @brief Variable that will be used to check which pipeline is used during computing 
        bool particleNeedReset = true;

        /// @brief Copy of the device from the app, here for convenience
        VkDevice device;
        /// @brief Copy of the compute queue from the app, here for convenience
        VkQueue queue;

        /// @brief The command buffers used for the compute phase
        std::vector<VkCommandBuffer> computeCommandBuffer;

        /// @brief Fence used for compute buffer synchronisation
        std::vector<VkFence> computeInFlightFences;
        /// @brief Semaphore used for synchronisation with the graphics pipeline
        std::vector<VkSemaphore> computeFinishedSemaphore;

        /// @brief Descriptor set layout for the compute pipeline, they are the same for both pipelines (init and update)
        VkDescriptorSetLayout descriptorSetLayout;
        /// @brief Descriptor sets for the compute pipeline, they are the same for both pipelines (init and update)
        std::vector<VkDescriptorSet> descriptorSets;

        /// @brief Pipeline layout for the initialization of the particles (or reset)
        VkPipelineLayout initPipelineLayout;
        /// @brief Pipeline for the initialization of the particles (or reset)
        VkPipeline initPipeline;

        /// @brief Pipeline layout for the update of the particles
        VkPipelineLayout updatePipelineLayout;
        /// @brief Pipeline for the update of the particles
        VkPipeline updatePipeline;

        void recordComputeCommandBuffer(VkCommandBuffer commandBuffer, VkPipeline computePipeline, VkPipelineLayout computePipelineLayout, int currentFrame);
        void createComputePipeline(VkPipeline &particlePipeline, VkPipelineLayout &particlePipelineLayout, const char *shaderPath);

    public:
        /// @brief Initialize the class with data from the app
        void init(VkDevice device, VkQueue computeQueue);

        void createCommandBuffers(VkCommandPool commandPool);
        void createSyncObjects();
        
        void createComputeDescriptorSetLayout();
        void createComputeDescriptorSets(std::vector<VkBuffer> shaderStorageBuffers, VkDescriptorPool descriptorPool, VkSampler textureSampler, VkImageView ringImageView);

        void createParticleInitPipeline();
        void createParticleUpdatePipeline();

        /// @brief Return the semaphore corresponding to the current frame in flight
        VkSemaphore getSemaphore(int currentFrame);

        /// @brief Record and submit the compute pipeline
        void updateParticle(int currentFrame);

        void cleanup();

};