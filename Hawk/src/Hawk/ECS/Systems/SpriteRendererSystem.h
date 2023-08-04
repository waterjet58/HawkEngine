#pragma once
#include "System.h"
#include "Hawk/ECS/ECSManager.hpp"
#include <vulkan/vulkan.h>

namespace Hawk {
	
	class SpriteRendererSystem : public System
	{
	public:
		void Init(std::shared_ptr<ECSManager> manager);

		void Update(float dt, VkCommandBuffer buffer, VkPipelineLayout layout);

	private:
		std::shared_ptr<ECSManager> _manager;
	};
	
}
