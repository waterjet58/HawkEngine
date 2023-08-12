#pragma once
#include "Platform/Vulkan/VulkanContext.h"
#include "VulkanRenderer.h"
#include "imgui.h"

namespace Hawk {

	class VulkanImGUI {

	public:

		VulkanImGUI(GLFWwindow* window, VulkanContext& context);
		~VulkanImGUI();

		void initImGUI();

	private:
		void createDescriptorPool();

		GLFWwindow* _window;
		VulkanContext& _context;
		VkDescriptorPool _descriptor;
	};

}