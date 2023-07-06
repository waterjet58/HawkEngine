#pragma once

#include "Hawk/Window.h"

namespace Hawk {

	class WindowsWindow : public Window
	{
	private:
		virtual void Init(const WindowProperties& properties);
		virtual void Shutdown();

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

		inline void SetEventCallback(const EventCallbackFunction& callback) override { _data.EventCallback = callback; }
	};

}

