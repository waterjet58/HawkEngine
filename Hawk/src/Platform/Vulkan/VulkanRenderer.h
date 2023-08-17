#pragma once
#include "VulkanSwapChain.h"
#include "Hawk/ECS/Systems/SpriteRendererSystem.h"
#include "Hawk/Core/Window.h"

namespace Hawk {

	class VulkanRenderer {
	public:

		VulkanRenderer();
		~VulkanRenderer();

		VulkanRenderer(const VulkanRenderer&) = delete;
		VulkanRenderer& operator=(const VulkanRenderer&) = delete;

		static void init(VulkanContext* context, Window* window);
		static void cleanup();
		static VkCommandBuffer beginFrame();
		static void endFrame();

		static void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		static void endSwapChainRenderPass(VkCommandBuffer commandBuffer);
	
		static VkRenderPass getSwapChainRenderPass() { return _swapChain->getRenderPass(); }
		static float getAspectRatio() { return _swapChain->extentAspectRatio(); }
		static bool isFrameInProgress() { return _isFrameStarted; }
		static VkCommandBuffer getCurrentCommandBuffer() {
			HWK_ASSERT(_isFrameStarted && "Cannot get command buffer when frame is not in progress");
			return _commandBuffers[_currentFrameIndex];
		}

		static int getFrameIndex() {
			HWK_ASSERT(_isFrameStarted && "Cannot get frame index when frame is not in progress");
			return _currentFrameIndex;
		}

		static uint32_t getImageCount() { return static_cast<uint32_t>(_swapChain->imageCount()); }

		static VulkanContext* GetContext() { return _context; }

	private:
		static VulkanContext* _context;
		static Window* _window;
		static std::unique_ptr<VulkanSwapChain> _swapChain;
		static std::shared_ptr<ECSManager> _ecsManager;
		static std::shared_ptr<SpriteRendererSystem> _spriteRenderer;
		static std::vector<VkCommandBuffer> _commandBuffers;

		static uint32_t _currentImageIndex;
		static int _currentFrameIndex;
		static bool _isFrameStarted;

		static void recreateSwapChain();
		static void createCommandBuffers();
		static void freeCommandBuffers();
	};

}