#include "hwkPrecompiledHeader.h"
#include "WindowsWindow.h"

#include "Hawk/Events/KeyEvent.h"
#include "Hawk/Events/ApplicationEvent.h"
#include "Hawk/Events/MouseEvent.h"
#include "Hawk/Renderer/GraphicsContext.h"
#include "Platform/Vulkan/VulkanContext.h"

#include <filesystem>
#include <backends/imgui_impl_glfw.h>

#include "Hawk/ImGui/Roboto-Regular.embed"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace Hawk {

	struct SimplePushConstantData {
		glm::mat2 transform{1.f};
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

	static bool _GLFWInit = false;

	VkRenderPass Window::GetRenderPass()
	{
		return VkRenderPass();
	}

	Window* Window::Create(const WindowProperties& properties)
	{
		return new WindowsWindow(properties);
	}

	WindowsWindow::WindowsWindow(const WindowProperties& properties)
	{
		Init(properties);
	}

	WindowsWindow::~WindowsWindow() 
	{
		Shutdown();
	}

	void WindowsWindow::Init(const WindowProperties& properties)
	{
		_data.Title = properties.Title;
		_data.Width = properties.Width;
		_data.Height = properties.Height;
		
		HWK_CORE_INFO("Creating Window {0} ({1}, {2})", properties.Title, properties.Width, properties.Height);

		

		if (!_GLFWInit)
		{
			int success = glfwInit();
			HWK_CORE_ASSERT(success, "Could not initialize GLFW!");
			_GLFWInit = true;
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		//Create the GLFW window
		_window = glfwCreateWindow((int)properties.Width, (int)properties.Height, _data.Title.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(_window, this);
		glfwSetFramebufferSizeCallback(_window, framebufferResizeCallback);

		//Init VulkanContext
		_context = new VulkanContext(_window);
		_context->init(_data.Width, _data.Height);

		_swapChain = std::make_unique<VulkanSwapChain>(*_context, GetExtent());

		loadModels();
		createPipelineLayout();
		recreateSwapChain();
		createCommandBuffers();

		//Set the current context to this current window
		glfwMakeContextCurrent(_window);
	
		glfwSetWindowUserPointer(_window, &_data);

		SetVSync(true); 

		//GLFW Callbacks and Window Event setup

		glfwSetWindowSizeCallback(_window, [](GLFWwindow* window, int width, int height) 
		{
			windowData& data = *(windowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(_window, [](GLFWwindow* window)
		{
			windowData& data = *(windowData*)glfwGetWindowUserPointer(window);

			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetKeyCallback(_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) 
		{
			windowData& data = *(windowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, 1);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetCharCallback(_window, [](GLFWwindow* window, unsigned int keycode)
		{
			windowData& data = *(windowData*)glfwGetWindowUserPointer(window);
			KeyTypedEvent event(keycode);
			data.EventCallback(event);
		});

		glfwSetMouseButtonCallback(_window, [](GLFWwindow* window, int button, int action, int mods) 
		{
			windowData& data = *(windowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetScrollCallback(_window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			windowData& data = *(windowData*)glfwGetWindowUserPointer(window);
			
			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(_window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			windowData& data = *(windowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

		//Need to init IMGUI after all of the call backs are created to not overwrite the callbacks that ImGui creates
		_vulkanImGUI = new VulkanImGUI(_window, *_context, *_swapChain, *_pipeline);

	}

	void WindowsWindow::initImGUI()
	{


	}

	void WindowsWindow::Shutdown()
	{
		vkDestroyPipelineLayout(_context->getDevice(), _pipelineLayout, _context->getAllocator());

		glfwDestroyWindow(_window);
		glfwTerminate();
	}

	void WindowsWindow::Update()
	{
		glfwPollEvents();
		drawFrame();
		//glfwSwapBuffers(_window);
	}

	void WindowsWindow::SetVSync(bool state)
	{
		/*if (state)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		_data.VSync = state;*/
	}

	void  WindowsWindow::createCommandBuffers()
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

	void WindowsWindow::freeCommandBuffers()
	{
		vkFreeCommandBuffers(_context->getDevice(), _context->getCommandPool(), static_cast<uint32_t>(_commandBuffers.size()), _commandBuffers.data());
		_commandBuffers.clear();
	}

	void WindowsWindow::recordCommandBuffer(int imageIndex)
	{
		static int frame = 0;
		frame = (frame + 1) % 2000;

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(_commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = _swapChain->getRenderPass();
		renderPassInfo.framebuffer = _swapChain->getFrameBuffer(imageIndex);
		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = _swapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(_commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(_swapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(_swapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, _swapChain->getSwapChainExtent() };
		vkCmdSetViewport(_commandBuffers[imageIndex], 0, 1, &viewport);
		vkCmdSetScissor(_commandBuffers[imageIndex], 0, 1, &scissor);

		_pipeline->bind(_commandBuffers[imageIndex]);
		_model->bind(_commandBuffers[imageIndex]);

		for (int i = 0; i < 5; i++)
		{
			SimplePushConstantData push{};
			push.offset = { -1.5f + frame * (.0009f * (i)), -0.5f + i * .2f};
			push.color = { 0.0f, 0.5f + (.1f * i) , 0.0f};

			vkCmdPushConstants(_commandBuffers[imageIndex], _pipelineLayout,
							   VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
							   0, sizeof(SimplePushConstantData), &push);

			_model->draw(_commandBuffers[imageIndex]);
		}

		ImDrawData *draw_data = ImGui::GetDrawData();
		ImGui_ImplVulkan_RenderDrawData(draw_data, _commandBuffers[imageIndex]);


		vkCmdEndRenderPass(_commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(_commandBuffers[imageIndex]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer!");
		}
	}



	void WindowsWindow::recreateSwapChain()
	{
		auto extent = GetExtent();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = GetExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(_context->getDevice());

		if (_swapChain == nullptr)
		{
			_swapChain = std::make_unique<VulkanSwapChain>(*_context, extent);
		}
		else
		{
			_swapChain = std::make_unique<VulkanSwapChain>(*_context, extent, std::move(_swapChain));
			if (_swapChain->imageCount() != _commandBuffers.size() && _commandBuffers.size() != 0)
			{
				ImGui_ImplVulkan_SetMinImageCount(_swapChain->imageCount());
				freeCommandBuffers();
				createCommandBuffers();
			}
		}

		createPipeline();
	}

	void WindowsWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto curWindow = reinterpret_cast<WindowsWindow*>(glfwGetWindowUserPointer(window));
		curWindow->_framebufferResize = true;
		curWindow->_data.Width = width;
		curWindow->_data.Height = height;
	}

	void WindowsWindow::loadModels()
	{
		std::vector<Model::Vertex> vertices{
		{{0.0f, -0.5f}, { 1.0f, 0.0f, 0.0f }}, //Red vertice
		{{0.5f, 0.5f}, { 0.0f, 1.0f, 0.0f }}, //Green vertice
		{{-0.5f, 0.5f}, { 0.0f, 0.0f, 1.0f }} //Blue vertice
		};
		_model = std::make_unique<Model>(*_context, vertices);
	}

	void  WindowsWindow::drawFrame()
	{
		uint32_t imageIndex;
		auto result = _swapChain->acquireNextImage(&imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("failed to acquire swap chain image");
		}

		recordCommandBuffer(imageIndex);
		result = _swapChain->submitCommandBuffers(&_commandBuffers[imageIndex], &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || wasWindowResized())
		{
			resetWindowResized();
			recreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present swap chain image!");
		}

	}

	void WindowsWindow::createPipelineLayout()
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);


		VkPipelineLayoutCreateInfo layoutCreateInfo{};
		layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutCreateInfo.setLayoutCount = 0;
		layoutCreateInfo.pSetLayouts = nullptr;
		layoutCreateInfo.pushConstantRangeCount = 1;
		layoutCreateInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(_context->getDevice(), &layoutCreateInfo, _context->getAllocator(), &_pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Pipeline layout failed to be created");
		}

	}

	void WindowsWindow::createPipeline()
	{
		PipelineConfigInfo pipelineConfig{};
		VulkanPipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = _swapChain->getRenderPass();
		pipelineConfig.pipelineLayout = _pipelineLayout;

		_pipeline = std::make_unique<VulkanPipeline>(*_context, "../Hawk/src/Shaders/simple_shader.vert.spv",
			"../Hawk/src/Shaders/simple_shader.frag.spv", pipelineConfig);

	}

	bool WindowsWindow::IsVSync() const
	{
		return _data.VSync;
	}

}