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
#include <Hawk/ECS/Components/Sprite.h>
#include <Hawk/ECS/Entity.h>
#include <Hawk/ECS/Systems/SpriteRendererSystem.h>
#include <Hawk/Application.h>

namespace Hawk {

	static bool _GLFWInit = false;

	Window* Window::Create(const std::string& Title ,const uint32_t Width, const uint32_t Height, VulkanContext& context)
	{
		WindowProperties properties(Title, Width, Height);
		return new WindowsWindow(properties, context);
	}

	WindowsWindow::WindowsWindow(const WindowProperties& properties, VulkanContext& context) : _context(context)
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

	}

	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(_window);
		glfwTerminate();
	}

	void WindowsWindow::Update()
	{
		glfwPollEvents();
		/*if (auto commandBuffer = _renderer->beginFrame())
		{
			_renderer->beginSwapChainRenderPass(commandBuffer);
			renderGameObjects(commandBuffer);
			_renderer->endSwapChainRenderPass(commandBuffer);
			_renderer->endFrame();
		}*/
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

	//ImDrawData *draw_data = ImGui::GetDrawData();
	//ImGui_ImplVulkan_RenderDrawData(draw_data, _commandBuffers[imageIndex]);

	void WindowsWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto curWindow = reinterpret_cast<WindowsWindow*>(glfwGetWindowUserPointer(window));
		curWindow->_framebufferResize = true;
		curWindow->_data.Width = width;
		curWindow->_data.Height = height;
	}

	/*
	void WindowsWindow::loadModels()
	{
		

		
	}
	*/

	

	bool WindowsWindow::IsVSync() const
	{
		return _data.VSync;
	}

}