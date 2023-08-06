#pragma once
#include "Platform/Vulkan/VulkanContext.h"
#include "VulkanRenderer.h"
#include "imgui.h"

namespace Hawk {

	class VulkanImGUI {

	public:

		VulkanImGUI(GLFWwindow* window, VulkanContext& context, VulkanRenderer& renderer);
		~VulkanImGUI();

		void initImGUI();

	private:
		void createDescriptorPool();

		GLFWwindow* _window;
		VulkanContext& _context;
		VulkanRenderer& _renderer;
		VkDescriptorPool _descriptor;
	};

}