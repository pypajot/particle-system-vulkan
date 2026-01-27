#include "ComputePipeline.hpp"
#include "ParticleSystemApplication.hpp"

void ComputePipeline::init(VkDevice appDevice, VkQueue computeQueue)
{
    device = appDevice;
    queue = computeQueue;
}

void ComputePipeline::createCommandBuffers(VkCommandPool commandPool)
{
    computeCommandBuffer.resize(NUMBER_OF_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = NUMBER_OF_FRAMES_IN_FLIGHT;

    VK_CHECK(vkAllocateCommandBuffers(device, &allocInfo, computeCommandBuffer.data()));
}

void ComputePipeline::createSyncObjects()
{
    computeFinishedSemaphore.resize(NUMBER_OF_FRAMES_IN_FLIGHT);
    computeInFlightFences.resize(NUMBER_OF_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (uint i = 0; i < NUMBER_OF_FRAMES_IN_FLIGHT; i++)
    {
        VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &computeFinishedSemaphore[i]));
        VK_CHECK(vkCreateFence(device, &fenceInfo, nullptr, &computeInFlightFences[i]));
    }
}

void ComputePipeline::createComputeDescriptorSetLayout()
{
    std::array<VkDescriptorSetLayoutBinding, 3> layoutBindings{};

    layoutBindings[0].binding = 0;
    layoutBindings[0].descriptorCount = 1;
    layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    layoutBindings[0].pImmutableSamplers = nullptr;
    layoutBindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    layoutBindings[1].binding = 1;
    layoutBindings[1].descriptorCount = 1;
    layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    layoutBindings[1].pImmutableSamplers = nullptr;
    layoutBindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    layoutBindings[2].binding = 2;
    layoutBindings[2].descriptorCount = 1;
    layoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    layoutBindings[2].pImmutableSamplers = nullptr;
    layoutBindings[2].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = layoutBindings.size();
    layoutInfo.pBindings = layoutBindings.data();

    VK_CHECK(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout));
}

void ComputePipeline::createComputeDescriptorSets(std::vector<VkBuffer> shaderStorageBuffers, VkDescriptorPool descriptorPool, VkSampler textureSampler, VkImageView ringImageView)
{
    std::vector<VkDescriptorSetLayout> layouts(NUMBER_OF_FRAMES_IN_FLIGHT, descriptorSetLayout);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(NUMBER_OF_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(NUMBER_OF_FRAMES_IN_FLIGHT);
    VK_CHECK(vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()));

    for (size_t i = 0; i < NUMBER_OF_FRAMES_IN_FLIGHT; i++)
    {
        std::array<VkWriteDescriptorSet, 3> descriptorWrites{};

        VkDescriptorBufferInfo storageBufferInfoLastFrame{};
        storageBufferInfoLastFrame.buffer = shaderStorageBuffers[(i - 1) % NUMBER_OF_FRAMES_IN_FLIGHT];
        storageBufferInfoLastFrame.offset = 0;
        storageBufferInfoLastFrame.range = sizeof(Particle) * PARTICLE_NUMBER;

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = descriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &storageBufferInfoLastFrame;

        VkDescriptorBufferInfo storageBufferInfoCurrentFrame{};
        storageBufferInfoCurrentFrame.buffer = shaderStorageBuffers[i];
        storageBufferInfoCurrentFrame.offset = 0;
        storageBufferInfoCurrentFrame.range = sizeof(Particle) * PARTICLE_NUMBER;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = descriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pBufferInfo = &storageBufferInfoCurrentFrame;

        VkDescriptorImageInfo ringImageInfo{};
        ringImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        ringImageInfo.imageView = ringImageView;
        ringImageInfo.sampler = textureSampler;

        descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[2].dstSet = descriptorSets[i];
        descriptorWrites[2].dstBinding = 2;
        descriptorWrites[2].dstArrayElement = 0;
        descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[2].descriptorCount = 1;
        descriptorWrites[2].pImageInfo = &ringImageInfo;

        vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}

void ComputePipeline::createParticleInitPipeline()
{
    createComputePipeline(initPipeline, initPipelineLayout, "shaders/particle/init.comp.spv");
}

void ComputePipeline::createParticleUpdatePipeline()
{
    createComputePipeline(updatePipeline, updatePipelineLayout, "shaders/particle/update.comp.spv");
}

void ComputePipeline::createComputePipeline(VkPipeline &particlePipeline, VkPipelineLayout &particlePipelineLayout, const char *shaderPath)
{
    auto computeShaderCode = readFile(shaderPath);

    VkShaderModule computeShaderModule = createShaderModule(device, computeShaderCode);

    VkPipelineShaderStageCreateInfo computeShaderStageInfo{};
    computeShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    computeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    computeShaderStageInfo.module = computeShaderModule;
    computeShaderStageInfo.pName = "main";

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

    VK_CHECK(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &particlePipelineLayout));
        
    VkComputePipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineInfo.layout = particlePipelineLayout;
    pipelineInfo.stage = computeShaderStageInfo;

    VK_CHECK(vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &particlePipeline));
    
    vkDestroyShaderModule(device, computeShaderModule, nullptr);
}


void ComputePipeline::updateParticle(int currentFrame)
{
    VK_CHECK(vkWaitForFences(device, 1, &computeInFlightFences[currentFrame], VK_TRUE, UINT64_MAX));
    VK_CHECK(vkResetFences(device, 1, &computeInFlightFences[currentFrame]));

    VK_CHECK(vkResetCommandBuffer(computeCommandBuffer[currentFrame], 0));
    if (particleNeedReset)
    {
        recordComputeCommandBuffer(computeCommandBuffer[currentFrame], initPipeline, initPipelineLayout, currentFrame);
        particleNeedReset = false;
    }
    else
        recordComputeCommandBuffer(computeCommandBuffer[currentFrame], updatePipeline, updatePipelineLayout, currentFrame);

    VkSubmitInfo submitInfo{};

    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &computeCommandBuffer[currentFrame];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &computeFinishedSemaphore[currentFrame];
    VK_CHECK(vkQueueSubmit(queue, 1, &submitInfo, computeInFlightFences[currentFrame]));
}

void ComputePipeline::recordComputeCommandBuffer(VkCommandBuffer commandBuffer, VkPipeline computePipeline, VkPipelineLayout computePipelineLayout, int currentFrame)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, 0);
    vkCmdDispatch(commandBuffer, PARTICLE_NUMBER / 64, 1, 1);

    VK_CHECK(vkEndCommandBuffer(commandBuffer));
}

void ComputePipeline::cleanup()
{
    vkDestroyPipelineLayout(device, initPipelineLayout, nullptr);
    vkDestroyPipeline(device, initPipeline, nullptr);

    vkDestroyPipelineLayout(device, updatePipelineLayout, nullptr);
    vkDestroyPipeline(device, updatePipeline, nullptr);

    for (uint i = 0; i < NUMBER_OF_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(device, computeFinishedSemaphore[i], nullptr);
        vkDestroyFence(device, computeInFlightFences[i], nullptr);
    }

    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
}

VkSemaphore ComputePipeline::getSemaphore(int currentFrame)
{
    return computeFinishedSemaphore[currentFrame];
}
