#include "hwkPrecompiledHeader.h"
#include "VulkanImGUI.h"
#include <backends/imgui_impl_glfw.h>

namespace Hawk {

	VulkanImGUI::VulkanImGUI(GLFWwindow* window,VulkanContext& context, VulkanSwapChain& swapChain, VulkanPipeline& pipeline)
		:_window(window), _context(context), _swapChain(swapChain), _pipeline(pipeline)
	{
		initImGUI();
	}

	VulkanImGUI::~VulkanImGUI() 
	{
		vkDestroyRenderPass(_context.getDevice(), _renderPass, _context.getAllocator());
	}

	void VulkanImGUI::initImGUI()
	{
		createDescriptorPool();
		createRenderPass();

		// Setup Dear ImGui context
		
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForVulkan(_window, true);
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = _context.getInstance();
		init_info.PhysicalDevice = _context.getPhysicalDevice();
		init_info.Device = _context.getDevice();
		init_info.QueueFamily = _context.findQueueFamilies(_context.getPhysicalDevice()).graphicsFamily;
		init_info.Queue = _context.getGraphicsQueue();
		init_info.PipelineCache = nullptr;
		init_info.DescriptorPool = _descriptor;
		init_info.Allocator = _context.getAllocator();
		init_info.MinImageCount = _swapChain.imageCount();
		init_info.ImageCount = _swapChain.imageCount();
		ImGui_ImplVulkan_Init(&init_info, _renderPass);

		VkCommandBuffer command_buffer = _context.beginSingleTimeCommands();
		ImGui_ImplVulkan_CreateFontsTexture(command_buffer);
		_context.endSingleTimeCommands(command_buffer);

		
	}

	void VulkanImGUI::createDescriptorPool()
	{
		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};
		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
		pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;
		vkCreateDescriptorPool(_context.getDevice(), &pool_info, _context.getAllocator(), &_descriptor);
	}

	void VulkanImGUI::createRenderPass()
	{
		VkAttachmentDescription attachment = {};
		attachment.format = _swapChain.getSwapChainImageFormat();
		attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference color_attachment = {};
		color_attachment.attachment = 0;
		color_attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &color_attachment;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;  // or VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		info.attachmentCount = 1;
		info.pAttachments = &attachment;
		info.subpassCount = 1;
		info.pSubpasses = &subpass;
		info.dependencyCount = 1;
		info.pDependencies = &dependency;
		if (vkCreateRenderPass(_context.getDevice(), &info, nullptr, &_renderPass) != VK_SUCCESS) {
			throw std::runtime_error("Could not create Dear ImGui's render pass");
		}
	}



}