#include "hwkPrecompiledHeader.h"
#include "VulkanRenderer.h"

namespace Hawk {

	VulkanRenderer::VulkanRenderer(VulkanContext* context) : _context(context) {}

	VulkanRenderer::~VulkanRenderer()
	{
		freeCommandBuffers();
	}

	void  VulkanRenderer::createCommandBuffers()
	{
		_commandBuffers.resize(_swapChain->imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = _context->getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(_commandBuffers.size());

		if (vkAllocateCommandBuffers(_context->getDevice(), &allocInfo, _commandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate command buffers!");
		}
	}

	void VulkanRenderer::freeCommandBuffers()
	{
		vkFreeCommandBuffers(_context->getDevice(), _context->getCommandPool(), static_cast<uint32_t>(_commandBuffers.size()), _commandBuffers.data());
		_commandBuffers.clear();
	}

	void VulkanRenderer::recreateSwapChain()
	{
		auto extent = Application::Get().GetWindow().GetExtent();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = Application::Get().GetWindow().GetExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(_context->getDevice());

		if (_swapChain == nullptr)
		{
			_swapChain = std::make_unique<VulkanSwapChain>(_context, extent);
		}
		else
		{
			_swapChain = std::make_unique<VulkanSwapChain>(_context, extent, std::move(_swapChain));
			if (_swapChain->imageCount() != _commandBuffers.size() && _commandBuffers.size() != 0)
			{
				ImGui_ImplVulkan_SetMinImageCount(_swapChain->imageCount());
				freeCommandBuffers();
				createCommandBuffers();
			}
		}

		
	}

	VkCommandBuffer VulkanRenderer::beginFrame()
	{
		HWK_CORE_ASSERT(!_isFrameStarted && "Cannot call begin frame while frame is already in progress");

		auto result = _swapChain->acquireNextImage(&_currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("failed to acquire swap chain image");
		}

		_isFrameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		return commandBuffer;
	}

	void VulkanRenderer::endFrame()
	{
		HWK_CORE_ASSERT(_isFrameStarted && "Can't call end frame when frame is not in progress");

		auto commandBuffer = getCurrentCommandBuffer();

		
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer!");
		}

		auto result = _swapChain->submitCommandBuffers(&commandBuffer, &_currentImageIndex);
		Window& window = Application::Get().GetWindow();

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.wasWindowResized())
		{
			window.resetWindowResized();
			recreateSwapChain();
		}

		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present swap chain image!");
		}

		_isFrameStarted = false;
	}

	void VulkanRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		HWK_CORE_ASSERT(_isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
		HWK_CORE_ASSERT(commanBuffer != getCurrentCommandBuffer() && "Can't begin renderpass on a command buffer from a different frame!");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = _swapChain->getRenderPass();
		renderPassInfo.framebuffer = _swapChain->getFrameBuffer(_currentImageIndex);
		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = _swapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(_swapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(_swapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, _swapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	}

	void VulkanRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		HWK_CORE_ASSERT(_isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
		HWK_CORE_ASSERT(commanBuffer != getCurrentCommandBuffer() && "Can't begin renderpass on a command buffer from a different frame!");

		vkCmdEndRenderPass(commandBuffer);
	}

}