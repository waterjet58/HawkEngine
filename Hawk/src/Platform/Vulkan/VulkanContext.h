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

#ifdef NDEBUG
		const bool enableValidationLayers = false;
#else
		const bool enableValidationLayers = true;
#endif

		VulkanContext(GLFWwindow* window);

		~VulkanContext();

		static void check_vk_result(VkResult err);

		void init() override;

		void cleanup() override;

		void createInstance();

		std::vector<const char*> getRequiredExtensions();

		void hasGflwRequiredInstanceExtensions();

		bool checkValidationLayerSupport();

		void selectGPU();

		void createLogicalDevice();

		void selectGraphicsQueue();

		void createDescriptorPool();

		void createWindowSurface();
		
		

		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

		void setupDebugMessenger();

	private:

		GLFWwindow* _window;

		VkAllocationCallbacks*		_Allocator = NULL;
		VkInstance					_Instance = VK_NULL_HANDLE;
		VkPhysicalDevice			_PhysicalDevice = VK_NULL_HANDLE;
		VkDevice					_Device = VK_NULL_HANDLE;
		uint32_t					_QueueFamily = (uint32_t)-1;
		VkQueue						_Queue = VK_NULL_HANDLE;
		VkDebugReportCallbackEXT	_DebugReport = VK_NULL_HANDLE;
		VkPipelineCache				_PipelineCache = VK_NULL_HANDLE;
		VkDescriptorPool			_DescriptorPool = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT	_DebugMessenger = VK_NULL_HANDLE;


		const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
		const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	};


}


