#pragma once

#include "Hawk/Window.h"
#include "Hawk/Renderer/GraphicsContext.h"
#include "GLFW/glfw3.h"

namespace Hawk {

	class WindowsWindow : public Window
	{
	private:
		GLFWwindow* _window;
		GraphicsContext* _context;
		virtual void Init(const WindowProperties& properties);
		virtual void Shutdown();

		struct windowData {
			std::string Title = "";
			unsigned int Width = 600, Height = 400;
			bool VSync = true;

			EventCallbackFunction EventCallback = NULL;
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
		virtual void* GetNativeWindow() const { return _window; }
	};

}

