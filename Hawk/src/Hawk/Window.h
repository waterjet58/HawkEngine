#pragma once

#include "Hawk/Core.h"
#include "Hawk/Events/Event.h"
#include "vulkan/vulkan.h"
#include "ECS/ECSManager.hpp"
#include "Platform/Vulkan/VulkanContext.h"
#include "Platform/Vulkan/VulkanRenderer.h"

namespace Hawk {

	struct WindowProperties
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;

		WindowProperties(const std::string& title = "Hawk Engine",
						 unsigned int width = 1280,
						 unsigned int height = 720)
		{
			Title = title;
			Width = width;
			Height = height;
		}

	};

	class Window
	{
	public:
		using EventCallbackFunction = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void Update() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;
		virtual VkExtent2D GetExtent() { return { GetWidth(), GetHeight() }; };

		virtual void SetEventCallback(const EventCallbackFunction& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		virtual bool wasWindowResized() const = 0;
		virtual void resetWindowResized() = 0;

		static Window* Create(const WindowProperties& properties = WindowProperties(), std::shared_ptr<ECSManager> manager = nullptr, VulkanContext& context, VulkanRenderer& renderer);
		
	};

}