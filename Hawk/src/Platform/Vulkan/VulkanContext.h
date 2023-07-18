#pragma once
#include "vulkan/vulkan.h"
#include "backends/imgui_impl_vulkan.h"
#include "Hawk/Renderer/GraphicsContext.h"
#include "GLFW/glfw3.h"
#include "Hawk/Window.h"

namespace Hawk {
	class VulkanContext : public GraphicsContext
	{

	public:

		VulkanContext(GLFWwindow* window);

		~VulkanContext();

		static void check_vk_result(VkResult err);

		void Init() override;
		
		void Cleanup() override;

	private:
		GLFWwindow* _window;
	};


}


