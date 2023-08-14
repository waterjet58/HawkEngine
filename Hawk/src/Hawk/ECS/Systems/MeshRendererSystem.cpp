#include "hwkPrecompiledHeader.h"
#include "MeshRendererSystem.h"
#include "Hawk/ECS/Components/Mesh.h"
#include <Platform/Vulkan/VulkanPipeline.h>


namespace Hawk {

	void MeshRendererSystem::Init(std::shared_ptr<ECSManager> manager, VulkanContext* context, VkRenderPass renderPass)
	{
		_context = context;
		_manager = manager;
		createPipelineLayout();
		createPipeline(renderPass);
	}

	void MeshRendererSystem::Update(Timestep dt, VkCommandBuffer buffer, const Camera& camera)
	{
		_pipeline->bind(buffer);

		auto projectionView = camera.getProjection() * camera.getView();

		for (auto const& entity : _entities)
		{
			auto& mesh = _manager->getComponent<Mesh>(entity);
			
			mesh.transform.rotation.y += dt * 20.f;

			MeshSimplePushConstantData push{};
			auto modelMatrix = mesh.transform.mat4();
			push.transform = projectionView * modelMatrix;
			push.modelMatrix = modelMatrix;

			vkCmdPushConstants(
				buffer,
				_pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(MeshSimplePushConstantData),
				&push);

			mesh.model->bind(buffer);
			mesh.model->draw(buffer);
			//HWK_CORE_INFO("Entity: {0}", entity);
		}

	}

	void MeshRendererSystem::createPipelineLayout()
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(MeshSimplePushConstantData);


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

	void MeshRendererSystem::createPipeline(VkRenderPass renderPass)
	{
		PipelineConfigInfo pipelineConfig{};
		VulkanPipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = _pipelineLayout;

		_pipeline = std::make_unique<VulkanPipeline>(*_context, "../Hawk/src/Shaders/simple_shader.vert.spv",
			"../Hawk/src/Shaders/simple_shader.frag.spv", pipelineConfig);

	}

	MeshRendererSystem::~MeshRendererSystem()
	{
		vkDestroyPipelineLayout(_context->getDevice(), _pipelineLayout, _context->getAllocator());
	}
}