#include "hwkPrecompiledHeader.h"
#include "WindowsWindow.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

#include "Hawk/Events/KeyEvent.h"
#include "Hawk/Events/ApplicationEvent.h"
#include "Hawk/Events/MouseEvent.h"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

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

	void WindowsWindow::SetupVulkan()
	{

	}

	void WindowsWindow::CleanupVulkan()
	{

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

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); //Remove OpenGL from init for GLFW
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); //Cancel resize for now TODO

		//Create the GLFW window
		_window = glfwCreateWindow((int)properties.Width, (int)properties.Height, _data.Title.c_str(), nullptr, nullptr);

		//Set the current context to this current window
		glfwMakeContextCurrent(_window);
		glfwSetWindowUserPointer(_window, &_data);

		//Intial setup for Vulkan
		SetupVulkan();

		//TODO for Vulkan
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

	}

	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(_window);
		glfwTerminate();
		CleanupVulkan();
	}

	void WindowsWindow::Update()
	{
		glfwPollEvents();
		glfwSwapBuffers(_window);
	}

	void WindowsWindow::SetVSync(bool state)
	{
		/*
		if (state)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		_data.VSync = state;
		*/
	}

	bool WindowsWindow::IsVSync() const
	{
		return _data.VSync;
	}

}