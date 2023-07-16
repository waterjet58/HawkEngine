#include "hwkPrecompiledHeader.h"
#include "VulkanInstance.h"
#include "backends/imgui_impl_vulkan.h"
#include "Hawk/Core.h"

void check_vk_result(VkResult err)
{
	if (err == 0)
		return;
	fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
	if (err < 0)
		abort();
}


namespace Hawk {

	VkAllocationCallbacks*	 _Allocator = NULL;
	VkInstance               _Instance = VK_NULL_HANDLE;
	VkPhysicalDevice         _PhysicalDevice = VK_NULL_HANDLE;
	VkDevice                 _Device = VK_NULL_HANDLE;
	uint32_t                 _QueueFamily = (uint32_t)-1;
	VkQueue                  _Queue = VK_NULL_HANDLE;
	VkDebugReportCallbackEXT _DebugReport = VK_NULL_HANDLE;
	VkPipelineCache          _PipelineCache = VK_NULL_HANDLE;
	VkDescriptorPool         _DescriptorPool = VK_NULL_HANDLE;
	int                      _MinImageCount = 2;
	bool                     _SwapChainRebuild = false;
	std::vector<std::vector<VkCommandBuffer>> _AllocatedCommandBuffers;
	std::vector<std::vector<std::function<void()>>> _ResourceFreeQueue;
	

	VulkanInstance* VulkanInstance::Create()
	{
		return new VulkanInstance();
	}

	void VulkanInstance::initVulkan(const char** extensions, uint32_t extensions_count)
	{

		VkResult err;
		//Create the Vulkan Instance
		{
		
			VkInstanceCreateInfo create_info = {};
			create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			create_info.enabledExtensionCount = extensions_count;
			create_info.ppEnabledExtensionNames = extensions;

#ifdef IMGUI_VULKAN_DEBUG_REPORT
			const char* layers[] = { "VK_LAYER_KHRONOS_validation" };
			create_info.enabledLayerCount = 1;
			create_info.ppEnabledLayerNames = layers;

			// Enable debug report extension (we need additional storage, so we duplicate the user array to add our new extension to it)
			const char** extensions_ext = (const char**)malloc(sizeof(const char*) * (extensions_count + 1));
			memcpy(extensions_ext, extensions, extensions_count * sizeof(const char*));
			extensions_ext[extensions_count] = "VK_EXT_debug_report";
			create_info.enabledExtensionCount = extensions_count + 1;
			create_info.ppEnabledExtensionNames = extensions_ext;

			// Create Vulkan Instance
			err = vkCreateInstance(&create_info, _Allocator, &_Instance);
			checkVkResult(err);
			free(extensions_ext);
#else
			err = vkCreateInstance(&create_info, _Allocator, &_Instance);
			check_vk_result(err);
			IM_UNUSED(_DebugReport);
#endif
		}

		//Select GPU
		{
			uint32_t gpu_count;
			err = vkEnumeratePhysicalDevices(_Instance, &gpu_count, NULL);
			check_vk_result(err);
			IM_ASSERT(gpu_count > 0);

			VkPhysicalDevice* gpus = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * gpu_count);
			err = vkEnumeratePhysicalDevices(_Instance, &gpu_count, gpus);
			check_vk_result(err);

			// If a number >1 of GPUs got reported, find discrete GPU if present, or use first one available. This covers
			// most common cases (multi-gpu/integrated+dedicated graphics). Handling more complicated setups (multiple
			// dedicated GPUs) is out of scope of this sample.
			int use_gpu = 0;
			for (int i = 0; i < (int)gpu_count; i++)
			{
				VkPhysicalDeviceProperties properties;
				vkGetPhysicalDeviceProperties(gpus[i], &properties);
				if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
				{
					use_gpu = i;
					break;
				}
			}

			_PhysicalDevice = gpus[use_gpu];
			free(gpus);
		}

		//Select graphics queue family
		{
			uint32_t count;
			vkGetPhysicalDeviceQueueFamilyProperties(_PhysicalDevice, &count, NULL);
			VkQueueFamilyProperties* queues = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * count);
			vkGetPhysicalDeviceQueueFamilyProperties(_PhysicalDevice, &count, queues);
			for (uint32_t i = 0; i < count; i++)
				if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					_QueueFamily = i;
					break;
				}
			free(queues);
			IM_ASSERT(_QueueFamily != (uint32_t)-1);
		}


		//Create Logical device with 1 queue
		{
			int device_extension_count = 1;
			const char* device_extensions[] = { "VK_KHR_swapchain" };
			const float queue_priority[] = { 1.0f };
			VkDeviceQueueCreateInfo queue_info[1] = {};
			queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queue_info[0].queueFamilyIndex = _QueueFamily;
			queue_info[0].queueCount = 1;
			queue_info[0].pQueuePriorities = queue_priority;
			VkDeviceCreateInfo device_create_info = {};
			device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			device_create_info.queueCreateInfoCount = sizeof(queue_info) / sizeof(queue_info[0]);
			device_create_info.pQueueCreateInfos = queue_info;
			device_create_info.enabledExtensionCount = device_extension_count;
			device_create_info.ppEnabledExtensionNames = device_extensions;
			err = vkCreateDevice(_PhysicalDevice, &device_create_info, _Allocator, &_Device);
			check_vk_result(err);
			vkGetDeviceQueue(_Device, _QueueFamily, 0, &_Queue);
		}

		//Create descriptor pool
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
			err = vkCreateDescriptorPool(_Device, &pool_info, _Allocator, &_DescriptorPool);
			check_vk_result(err);
		}

		HWK_CORE_INFO("Extensions: {0}", extensions_count);

	}

	void VulkanInstance::cleanupVulkan()
	{
		vkDestroyDescriptorPool(_Device, _DescriptorPool, _Allocator);

#ifdef IMGUI_VULKAN_DEBUG_REPORT
		// Remove the debug report callback
		auto vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(_Instance, "vkDestroyDebugReportCallbackEXT");
		vkDestroyDebugReportCallbackEXT(_Instance, _DebugReport, _Allocator);
#endif // IMGUI_VULKAN_DEBUG_REPORT

		vkDestroyDevice(_Device, _Allocator);
		vkDestroyInstance(_Instance, _Allocator);
	}
}