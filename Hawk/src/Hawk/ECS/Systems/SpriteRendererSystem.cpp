#include "hwkPrecompiledHeader.h"
#include "SpriteRendererSystem.h"
#include "Hawk/ECS/Components/Sprite.h"
#include <Platform/Vulkan/VulkanPipeline.h>

namespace Hawk {
	
	void SpriteRendererSystem::Init(std::shared_ptr<ECSManager> manager)
	{
		_manager = manager;
	}

	void SpriteRendererSystem::Update(float dt, VkCommandBuffer buffer, VkPipelineLayout layout)
	{

		for (auto const& entity : _entities)
		{
			auto& sprite = _manager->getComponent<Sprite>(entity);

			sprite.transform.rotation += .01f;


			SimplePushConstantData push{};
			push.offset = sprite.transform.position;
			push.color = sprite.color;
			push.transform = sprite.transform.mat2();

			vkCmdPushConstants(
				buffer,
				layout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push);

			sprite.model->bind(buffer);
			sprite.model->draw(buffer);
			HWK_CORE_INFO("Render Entity: {0}", entity);
		}
	}

}