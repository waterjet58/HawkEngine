#pragma once

#include "vulkan/vulkan.h"
#include "Hawk/Core/Camera.h"

namespace Hawk {

	struct FrameData
	{
		int frameIndex;
		float deltaTime;
		VkCommandBuffer commandbuffer;
		Camera camera;
		VkDescriptorSet descriptorSet;
	};

}