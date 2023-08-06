#pragma once
#include "System.h"
#include "Hawk/ECS/ECSManager.hpp"
#include <vulkan/vulkan.h>
#include <Platform/Vulkan/VulkanPipeline.h>

namespace Hawk {
	
	class SpriteRendererSystem : public System
	{
	public:
		void Init(std::shared_ptr<ECSManager> manager, VulkanContext* context, VkRenderPass renderPass);

		void Update(float dt, VkCommandBuffer buffer);

		~SpriteRendererSystem();
	private:
		std::shared_ptr<ECSManager> _manager;
		std::unique_ptr<VulkanPipeline> _pipeline;
		VkPipelineLayout _pipelineLayout;

		VulkanContext* _context;

		void createPipelineLayout();

		void createPipeline(VkRenderPass renderPass);
	};
	
}
