#pragma once

#include "Hawk/Window.h"

#include "GLFW/glfw3.h"

namespace Hawk {

	class WindowsWindow : public Window
	{
	private:
		GLFWwindow* _window;
		virtual void Init(const WindowProperties& properties);
		virtual void Shutdown();
		virtual void SetupVulkan();
		virtual void CleanupVulkan();

		struct windowData {
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFunction EventCallback;
		};

		windowData _data;
	public:
		WindowsWindow(const WindowProperties& properties);
		virtual ~WindowsWindow();

		void Update() override;

		inline unsigned int GetWidth() const override { return _data.Width; }
		inline unsigned int GetHeight() const override { return _data.Height; }
		

		inline void SetEventCallback( const EventCallbackFunction& callback) override { _data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;
	};

}

