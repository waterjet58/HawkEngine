#pragma once
#include "Platform/Vulkan/VulkanContext.h"
#include "Platform/Vulkan/VulkanSwapChain.h"
#include "Platform/Vulkan/VulkanPipeline.h"
#include "imgui.h"

namespace Hawk {

	class VulkanImGUI {

	public:

		VulkanImGUI(GLFWwindow* window, VulkanContext& context, VulkanSwapChain& swapChain, VulkanPipeline& pipeline);
		~VulkanImGUI();

		void initImGUI();

	private:
		void createDescriptorPool();
		void createRenderPass();

		GLFWwindow* _window;
		VulkanContext& _context;
		VulkanSwapChain& _swapChain;
		VulkanPipeline& _pipeline;
		VkDescriptorPool _descriptor;
		VkRenderPass _renderPass;
	};

}