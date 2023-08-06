#pragma once
#include "VulkanSwapChain.h"
#include "Hawk/ECS/Systems/SpriteRendererSystem.h"
#include "Hawk/Window.h"

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
		bool isFrameInProgress() const { return _isFrameStarted; }
		VkCommandBuffer getCurrentCommandBuffer() const {
			HWK_ASSERT(_isFrameStarted && "Cannot get command buffer when frame is not in progress");
			return _commandBuffers[_currentImageIndex];
		}

		uint32_t getImageCount() const { return static_cast<uint32_t>(_swapChain->imageCount()); }

	private:
		VulkanContext& _context;
		Window* _window;
		std::unique_ptr<VulkanSwapChain> _swapChain;
		std::shared_ptr<ECSManager> _ecsManager;
		std::shared_ptr<SpriteRendererSystem> _spriteRenderer;
		std::vector<VkCommandBuffer> _commandBuffers;

		uint32_t _currentImageIndex{false};
		bool _isFrameStarted{0};

		void recreateSwapChain();
		void createCommandBuffers();
		void freeCommandBuffers();
	};

}