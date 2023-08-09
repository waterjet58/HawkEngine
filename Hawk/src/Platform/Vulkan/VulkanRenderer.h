#pragma once
#include "VulkanSwapChain.h"
#include "Hawk/ECS/Systems/SpriteRendererSystem.h"
#include "Hawk/Core/Window.h"

namespace Hawk {

	class VulkanRenderer {
	public:
		VulkanRenderer(VulkanContext& context, Window* window);
		~VulkanRenderer();

		void init();

		VulkanRenderer(const VulkanRenderer&) = delete;
		VulkanRenderer& operator=(const VulkanRenderer&) = delete;

		VkCommandBuffer beginFrame();
		void endFrame();

		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);
	
		VkRenderPass getSwapChainRenderPass() const { return _swapChain->getRenderPass(); }
		float getAspectRatio() const { return _swapChain->extentAspectRatio(); }
		bool isFrameInProgress() const { return _isFrameStarted; }
		VkCommandBuffer getCurrentCommandBuffer() {
			HWK_ASSERT(_isFrameStarted && "Cannot get command buffer when frame is not in progress");
			return _commandBuffers[_currentFrameIndex];
		}

		int getFrameIndex() const {
			HWK_ASSERT(_isFrameStarted && "Cannot get frame index when frame is not in progress");
			return _currentFrameIndex;
		}

		uint32_t getImageCount() const { return static_cast<uint32_t>(_swapChain->imageCount()); }

	private:
		VulkanContext& _context;
		Window* _window;
		std::unique_ptr<VulkanSwapChain> _swapChain;
		std::shared_ptr<ECSManager> _ecsManager;
		std::shared_ptr<SpriteRendererSystem> _spriteRenderer;
		std::vector<VkCommandBuffer> _commandBuffers;

		uint32_t _currentImageIndex;
		int _currentFrameIndex{0};
		bool _isFrameStarted;

		void recreateSwapChain();
		void createCommandBuffers();
		void freeCommandBuffers();
	};

}