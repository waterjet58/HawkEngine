#pragma once

#include "vulkan/vulkan.h"
#include "backends/imgui_impl_vulkan.h"
#include "Hawk/Renderer/GraphicsContext.h"
#include "GLFW/glfw3.h"
#include "Hawk/Window.h"



namespace Hawk {

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	struct QueueFamilyIndices {
		uint32_t graphicsFamily;
		uint32_t presentFamily;
		bool graphicsFamilyHasValue = false;
		bool presentFamilyHasValue = false;
		bool isComplete() { return graphicsFamilyHasValue && presentFamilyHasValue; }
	};

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

		void init(uint32_t width, uint32_t height) override;

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

		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

		VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		QueueFamilyIndices findPhysicalQueueFamilies() { return findQueueFamilies(_PhysicalDevice); }

		void createImageWithInfo(const VkImageCreateInfo& imageInfo, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		
		VkDevice getDevice() { return _Device; }

		VkSurfaceKHR getSurface() { return _Surface; }

		VkAllocationCallbacks* getAllocator(){ return _Allocator; }

		SwapChainSupportDetails getSwapChainSupport() { return querySwapChainSupport(_PhysicalDevice); }

		VkQueue getGraphicsQueue() { return _graphicsQueue; }

		VkQueue getPresentQueue() { return _presentQueue; }

		VkPhysicalDevice getPhysicalDevice() { return _PhysicalDevice;  }

		VkInstance getInstance() { return _Instance; }

		void createCommandPool();

		VkCommandPool getCommandPool() { return _commandPool; }

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
		VkCommandPool				_commandPool;

		VkDevice _device;
		VkSurfaceKHR _Surface;
		VkQueue _graphicsQueue;
		VkQueue _presentQueue;

		const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
		const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	};


}


