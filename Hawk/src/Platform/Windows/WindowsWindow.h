#pragma once

#include "Hawk/Core/Window.h"
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
		WindowsWindow(const WindowProperties& properties, VulkanContext& _context);
		virtual ~WindowsWindow();

		void Update() override;

		inline unsigned int GetWidth() const override { return _data.Width; }
		inline unsigned int GetHeight() const override { return _data.Height; }
		VkExtent2D GetExtent() override { return { static_cast<uint32_t>(_data.Width), static_cast<uint32_t>(_data.Height) }; }

		inline void SetEventCallback( const EventCallbackFunction& callback) override { _data.EventCallback = callback; }
		inline bool wasWindowResized() override { return _framebufferResize; }
		inline void resetWindowResized() override { _framebufferResize = false; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;
		virtual void* GetNativeWindow() const { return _window; }

	private:
		GLFWwindow* _window;
		VulkanContext& _context;
		bool _framebufferResize = false;

		virtual void Init(const WindowProperties& properties);
		virtual void Shutdown();
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

		struct windowData {
			std::string Title = "";
			unsigned int Width = 600, Height = 400;
			bool VSync = true;

			EventCallbackFunction EventCallback = NULL;
		};

		windowData _data;
	};

}

