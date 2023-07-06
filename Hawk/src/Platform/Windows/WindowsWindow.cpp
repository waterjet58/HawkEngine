#include "hwkPrecompiledHeader.h"
#include "WindowsWindow.h"

namespace Hawk {

	static bool _GLFWInit = false;

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

		_window = glfwCreateWindow((int)properties.Width, (int)properties.Height, _data.Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(_window);
		glfwSetWindowUserPointer(_window, &_data);
		SetVSync(true);
	}

	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(_window);
	}

	void WindowsWindow::Update()
	{
		glfwPollEvents();
		glfwSwapBuffers(_window);
	}

	void WindowsWindow::SetVSync(bool state)
	{
		if (state)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		_data.VSync = state;
	}

	bool WindowsWindow::IsVSync() const
	{
		return _data.VSync;
	}

}