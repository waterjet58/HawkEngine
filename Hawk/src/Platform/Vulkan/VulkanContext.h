#pragma once
#include "vulkan/vulkan.h"
#include "backends/imgui_impl_vulkan.h"
#include "Hawk/Renderer/GraphicsContext.h"
#include "GLFW/glfw3.h"

namespace Hawk {
	class VulkanContext : public GraphicsContext
	{

	public:

		GLFWwindow* _window;
		ImGui_ImplVulkanH_Window _MainWindowData;

		VulkanContext(GLFWwindow* window);
		~VulkanContext();
		static void check_vk_result(VkResult err);

		void Init() override;
		
		void Cleanup() override;

		static void SetupVulkanWindow(ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int width, int height);


		static void FlushCommandBuffer(VkCommandBuffer commandBuffer);

		ImGui_ImplVulkanH_Window* GetWindowData();
		static void FrameRender(ImGui_ImplVulkanH_Window* wd, ImDrawData* draw_data);
		static void FramePresent(ImGui_ImplVulkanH_Window* wd);
	};
}


