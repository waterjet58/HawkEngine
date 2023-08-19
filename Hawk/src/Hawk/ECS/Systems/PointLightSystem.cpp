#include "hwkPrecompiledHeader.h"
#include "PointLightSystem.h"
#include <Platform/Vulkan/VulkanPipeline.h>
#include <Hawk/ECS/Components/PointLight.h>

namespace Hawk {

	void PointLightSystem::Init(std::shared_ptr<ECSManager> manager, VulkanContext* context, VkRenderPass renderPass, VkDescriptorSetLayout descriptorSetLayout)
	{
		_context = context;
		_manager = manager;
		_descriptorLayout = descriptorSetLayout;
		createPipelineLayout(descriptorSetLayout);
		createPipeline(renderPass);
	}

	void PointLightSystem::Update(Timestep dt, FrameData& frameData)
	{
		_pipeline->bind(frameData.commandbuffer);

		vkCmdBindDescriptorSets(
			frameData.commandbuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			_pipelineLayout,
			0, 1,
			&frameData.descriptorSet,
			0,
			nullptr);

		for (auto const& entity : _entities)
		{
			auto& mesh = _manager->getComponent<PointLight>(entity);
			vkCmdDraw(frameData.commandbuffer, 6, 1, 0, 0);
		}

	}

	void PointLightSystem::createPipelineLayout(VkDescriptorSetLayout descriptorLayout)
	{
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ descriptorLayout };

		VkPipelineLayoutCreateInfo layoutCreateInfo{};
		layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutCreateInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		layoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();
		layoutCreateInfo.pushConstantRangeCount = 0;
		layoutCreateInfo.pPushConstantRanges = nullptr;
		if (vkCreatePipelineLayout(_context->getDevice(), &layoutCreateInfo, _context->getAllocator(), &_pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Pipeline layout failed to be created");
		}
	}

	void PointLightSystem::createPipeline(VkRenderPass renderPass)
	{
		PipelineConfigInfo pipelineConfig{};
		VulkanPipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = _pipelineLayout;

		_pipeline = std::make_unique<VulkanPipeline>(*_context, "../Hawk/src/Shaders/point_light_shader.vert.spv",
			"../Hawk/src/Shaders/point_light_shader.frag.spv", pipelineConfig);

	}

	void PointLightSystem::cleanup()
	{
		_pipeline->cleanup();
		vkDestroyPipelineLayout(_context->getDevice(), _pipelineLayout, _context->getAllocator());
	}

	PointLightSystem::~PointLightSystem()
	{

	}
}