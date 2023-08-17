#include "hwkPrecompiledHeader.h"
#include "MeshRendererSystem.h"
#include "Hawk/ECS/Components/Mesh.h"
#include <Platform/Vulkan/VulkanPipeline.h>


namespace Hawk {

	void MeshRendererSystem::Init(std::shared_ptr<ECSManager> manager, VulkanContext* context, VkRenderPass renderPass, VkDescriptorSetLayout descriptorSetLayout)
	{
		_context = context;
		_manager = manager;
		_descriptorLayout = descriptorSetLayout;
		createPipelineLayout(descriptorSetLayout);
		createPipeline(renderPass);
	}

	void MeshRendererSystem::Update(Timestep dt, FrameData& frameData)
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
			auto& mesh = _manager->getComponent<Mesh>(entity);
			
			//mesh.transform.rotation.y -= dt * 10.f;

			MeshSimplePushConstantData push{};
			push.modelMatrix = mesh.transform.mat4();
			push.normalMatrix = mesh.transform.normalMatrix();

			vkCmdPushConstants(
				frameData.commandbuffer,
				_pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(MeshSimplePushConstantData),
				&push);

			mesh.model->bind(frameData.commandbuffer);
			mesh.model->draw(frameData.commandbuffer);
			//HWK_CORE_INFO("Entity: {0}", entity);
		}

	}

	void MeshRendererSystem::createPipelineLayout(VkDescriptorSetLayout descriptorLayout)
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(MeshSimplePushConstantData);

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{descriptorLayout};

		VkPipelineLayoutCreateInfo layoutCreateInfo{};
		layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutCreateInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		layoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();
		layoutCreateInfo.pushConstantRangeCount = 1;
		layoutCreateInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(_context->getDevice(), &layoutCreateInfo, _context->getAllocator(), &_pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Pipeline layout failed to be created");
		}
	}

	void MeshRendererSystem::createPipeline(VkRenderPass renderPass)
	{
		PipelineConfigInfo pipelineConfig{};
		VulkanPipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = _pipelineLayout;

		_pipeline = std::make_unique<VulkanPipeline>(*_context, "../Hawk/src/Shaders/simple_shader.vert.spv",
			"../Hawk/src/Shaders/simple_shader.frag.spv", pipelineConfig);

	}

	void MeshRendererSystem::cleanup()
	{
		_pipeline->cleanup();
		vkDestroyPipelineLayout(_context->getDevice(), _pipelineLayout, _context->getAllocator());
	}

	MeshRendererSystem::~MeshRendererSystem()
	{
		
	}
}