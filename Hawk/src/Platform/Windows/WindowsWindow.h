#pragma once

#include "Hawk/Window.h"
#include "Hawk/Renderer/GraphicsContext.h"
#include "GLFW/glfw3.h"
#include "Platform/Vulkan/VulkanPipeline.h"
#include "Platform/Vulkan/VulkanSwapChain.h"
#include "Hawk/Renderer/Model.h"
#include "Platform/Vulkan/VulkanImGUI.h"

namespace Hawk {

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProperties& properties);
		virtual ~WindowsWindow();

		void Update() override;

		inline unsigned int GetWidth() const override { return _data.Width; }
		inline unsigned int GetHeight() const override { return _data.Height; }
		VkExtent2D GetExtent() override { return { static_cast<uint32_t>(_data.Width), static_cast<uint32_t>(_data.Height) }; }

		inline void SetEventCallback( const EventCallbackFunction& callback) override { _data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void sierpinski(std::vector<Model::Vertex>& vertices, int depth, glm::vec2 left, glm::vec2 right, glm::vec2 top);
		void drawFrame();
		bool IsVSync() const override;
		virtual void* GetNativeWindow() const { return _window; }

	private:
		GLFWwindow* _window;
		VulkanContext* _context;
		VulkanSwapChain* _swapChain;
		std::unique_ptr<VulkanPipeline> _pipeline;
		std::unique_ptr<Model> _model;
		VkPipelineLayout _pipelineLayout;
		std::vector<VkCommandBuffer> _commandBuffers;
		VkCommandBuffer _imGuiBuffer;
		VulkanImGUI* _vulkanImGUI;


		virtual void Init(const WindowProperties& properties);
		void initImGUI();
		void loadModels();
		virtual void Shutdown();

		struct windowData {
			std::string Title = "";
			unsigned int Width = 600, Height = 400;
			bool VSync = true;

			EventCallbackFunction EventCallback = NULL;
		};

		windowData _data;
	};

}

