#include "hwkPrecompiledHeader.h"
#include "EditorGridSystem.h"
#include "Hawk/ECS/Components/Billboard.h"
#include <Platform/Vulkan/VulkanPipeline.h>


namespace Hawk {

	void EditorGridSystem::Init(std::shared_ptr<ECSManager> manager, VulkanContext* context, VkRenderPass renderPass, VkDescriptorSetLayout descriptorSetLayout)
	{
		_context = context;
		_manager = manager;
		_descriptorLayout = descriptorSetLayout;
		createPipelineLayout(descriptorSetLayout);
		createPipeline(renderPass);
	}

	void EditorGridSystem::Update(Timestep dt, FrameData& frameData)
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

		vkCmdDraw(frameData.commandbuffer, 6, 1, 0, 0);

	}

	void EditorGridSystem::createPipelineLayout(VkDescriptorSetLayout descriptorLayout)
	{
		//VkPushConstantRange pushConstantRange{};
		//pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		//pushConstantRange.offset = 0;
		//pushConstantRange.size = sizeof(MeshSimplePushConstantData);

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

	void EditorGridSystem::createPipeline(VkRenderPass renderPass)
	{
		PipelineConfigInfo pipelineConfig{};
		VulkanPipeline::defaultPipelineConfigInfo(pipelineConfig);
		VulkanPipeline::enableAlphaBlending(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = _pipelineLayout;

		pipelineConfig.attributeDescriptions.clear();
		pipelineConfig.bindingDescriptions.clear();

		_pipeline = std::make_unique<VulkanPipeline>(*_context,
			"../Hawk/src/Shaders/editor_grid_shader.vert.spv",
			"../Hawk/src/Shaders/editor_grid_shader.frag.spv", pipelineConfig);

	}

	void EditorGridSystem::cleanup()
	{
		_pipeline->cleanup();
		vkDestroyPipelineLayout(_context->getDevice(), _pipelineLayout, _context->getAllocator());
	}

	EditorGridSystem::~EditorGridSystem()
	{

	}
}