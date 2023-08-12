#include "hwkPrecompiledHeader.h"
#include "VulkanImGUI.h"
#include <backends/imgui_impl_glfw.h>

namespace Hawk {

	VulkanImGUI::VulkanImGUI(GLFWwindow* window, VulkanContext& context)
		:_window(window), _context(context) { }

	VulkanImGUI::~VulkanImGUI() 
	{
		vkDestroyDescriptorPool(_context.getDevice(), _descriptor, _context.getAllocator());
	}

	void VulkanImGUI::initImGUI()
	{
		createDescriptorPool();

		// Setup Dear ImGui context
		
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows


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
		init_info.MinImageCount = static_cast<uint32_t>(VulkanRenderer::getImageCount());
		init_info.ImageCount = static_cast<uint32_t>(VulkanRenderer::getImageCount());
		ImGui_ImplVulkan_Init(&init_info, VulkanRenderer::getSwapChainRenderPass());

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




}