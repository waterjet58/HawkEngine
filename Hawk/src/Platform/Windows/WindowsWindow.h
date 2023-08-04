#pragma once

#include "Hawk/Window.h"
#include "Hawk/Renderer/GraphicsContext.h"
#include "GLFW/glfw3.h"
#include "Platform/Vulkan/VulkanPipeline.h"
#include "Hawk/Renderer/Model.h"
#include "Platform/Vulkan/VulkanImGUI.h"
#include <Hawk/ECS/ECSManager.hpp>
#include "Hawk/ECS/Systems/SpriteRendererSystem.h"

namespace Hawk {

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProperties& properties, std::shared_ptr<ECSManager> manager, VulkanContext& context, VulkanRenderer& renderer);
		virtual ~WindowsWindow();

		void Update() override;

		inline unsigned int GetWidth() const override { return _data.Width; }
		inline unsigned int GetHeight() const override { return _data.Height; }
		VkExtent2D GetExtent() override { return { static_cast<uint32_t>(_data.Width), static_cast<uint32_t>(_data.Height) }; }

		inline void SetEventCallback( const EventCallbackFunction& callback) override { _data.EventCallback = callback; }
		inline bool wasWindowResized() const override { return _framebufferResize; }
		inline void resetWindowResized() override { _framebufferResize = false; }
		void SetVSync(bool enabled) override;

		bool IsVSync() const override;
		virtual void* GetNativeWindow() const { return _window; }

	private:
		GLFWwindow* _window;
		VulkanContext& _context;
		VulkanRenderer& _renderer;
		std::unique_ptr<VulkanPipeline> _pipeline;
		std::shared_ptr<Model> _model;
		std::shared_ptr<ECSManager> _ecsManager;
		std::shared_ptr<SpriteRendererSystem> _spriteRenderer;
		VkPipelineLayout _pipelineLayout;
		VkCommandBuffer _imGuiBuffer;
		VulkanImGUI* _vulkanImGUI;
		bool _framebufferResize = false;

		virtual void Init(const WindowProperties& properties);
		void initImGUI();
		void loadModels();
		virtual void Shutdown();
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
		void renderGameObjects(VkCommandBuffer buffer);
		void createPipelineLayout();
		void createPipeline();

		struct windowData {
			std::string Title = "";
			unsigned int Width = 600, Height = 400;
			bool VSync = true;

			EventCallbackFunction EventCallback = NULL;
		};

		windowData _data;
	};

}

