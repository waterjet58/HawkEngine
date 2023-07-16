#pragma once



namespace Hawk {
	class VulkanInstance
	{
	private:
		

	public:
		static VulkanInstance* Create();

		void initVulkan(const char** extensions, uint32_t extensisons_count);
		
		void cleanupVulkan();

	};
}


