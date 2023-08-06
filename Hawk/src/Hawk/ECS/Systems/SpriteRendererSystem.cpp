#include "hwkPrecompiledHeader.h"
#include "SpriteRendererSystem.h"
#include "Hawk/ECS/Components/Sprite.h"
#include <Platform/Vulkan/VulkanPipeline.h>

namespace Hawk {
	
	void SpriteRendererSystem::Init(std::shared_ptr<ECSManager> manager, VulkanContext* context, VkRenderPass renderPass)
	{
		_context = context;
		_manager = manager;
		createPipelineLayout();
		createPipeline(renderPass);
	}

	

	void SpriteRendererSystem::Update(float dt, VkCommandBuffer buffer)
	{
		_pipeline->bind(buffer);

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
				_pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push);

			sprite.model->bind(buffer);
			sprite.model->draw(buffer);
			HWK_CORE_INFO("Render Entity: {0}", entity);
		}
	}

	void SpriteRendererSystem::createPipelineLayout()
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);


		VkPipelineLayoutCreateInfo layoutCreateInfo{};
		layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutCreateInfo.setLayoutCount = 0;
		layoutCreateInfo.pSetLayouts = nullptr;
		layoutCreateInfo.pushConstantRangeCount = 1;
		layoutCreateInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(_context->getDevice(), &layoutCreateInfo, _context->getAllocator(), &_pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Pipeline layout failed to be created");
		}
	}

	void SpriteRendererSystem::createPipeline(VkRenderPass renderPass)
	{
		PipelineConfigInfo pipelineConfig{};
		VulkanPipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = _pipelineLayout;

		_pipeline = std::make_unique<VulkanPipeline>(*_context, "../Hawk/src/Shaders/simple_shader.vert.spv",
			"../Hawk/src/Shaders/simple_shader.frag.spv", pipelineConfig);

	}

	SpriteRendererSystem::~SpriteRendererSystem()
	{
		vkDestroyPipelineLayout(_context->getDevice(), _pipelineLayout, _context->getAllocator());
	}
}