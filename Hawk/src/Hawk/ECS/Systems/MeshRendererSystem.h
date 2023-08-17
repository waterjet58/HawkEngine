#pragma once
#include "System.h"
#include "Hawk/ECS/ECSManager.hpp"
#include <vulkan/vulkan.h>
#include <Platform/Vulkan/VulkanPipeline.h>
#include "Hawk/Core/Timestep.h"
#include "Hawk/Core/Camera.h"
#include "Hawk/Renderer/FrameData.h"

namespace Hawk {

	class MeshRendererSystem : public System
	{
	public:
		void Init(std::shared_ptr<ECSManager> manager, VulkanContext* context, VkRenderPass renderPass, VkDescriptorSetLayout descriptorSetLayout);

		void Update(Timestep dt, FrameData& frameData);

		~MeshRendererSystem();
	private:
		std::shared_ptr<ECSManager> _manager;
		std::unique_ptr<VulkanPipeline> _pipeline;
		VkPipelineLayout _pipelineLayout;
		VkDescriptorSetLayout _descriptorLayout;

		VulkanContext* _context;

		void createPipelineLayout(VkDescriptorSetLayout descriptorLayout);

		void createPipeline(VkRenderPass renderPass);
	};

}