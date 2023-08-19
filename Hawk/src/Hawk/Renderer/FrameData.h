#pragma once

#include "vulkan/vulkan.h"
#include "Hawk/Core/Camera.h"

namespace Hawk {

#define MAX_LIGHTS 50

	struct FrameData
	{
		int frameIndex;
		float deltaTime;
		VkCommandBuffer commandbuffer;
		Camera camera;
		VkDescriptorSet descriptorSet;
	};

	struct GlobalUBO
	{
		glm::mat4 projection{ 1.f };
		glm::mat4 view{ 1.f };
		glm::mat4 projectionView{ 1.f };
		glm::vec4 ambientLight{ 1.f, 1.f, 1.f, .02f }; // {R, G, B, Intensity}
		PointLight pointLights[MAX_LIGHTS];
		int numActiveLights;
	};

}