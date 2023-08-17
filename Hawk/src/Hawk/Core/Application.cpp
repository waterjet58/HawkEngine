#include "hwkPrecompiledHeader.h"
#include "Application.h"
#include "Hawk/Events/Event.h"
#include "Hawk/Events/ApplicationEvent.h"
#include "Hawk/Core/Log.h"
//#include "ImGui/ImGUILayer.h"
//#include "glad/glad.h"
#include "Input.h"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#include "Hawk/ECS/Components/Sprite.h"
#include "Hawk/Math/Quaternion.h"
#include "Timestep.h"
#include <Hawk/ECS/Components/Mesh.h>
#include "Hawk/Core/KeyCodes.h"

namespace Hawk {

#define BIND_EVENT_FUNCTION(x) std::bind(&x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;
	

	Application::Application()
	{
		HWK_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		_window->SetEventCallback(BIND_EVENT_FUNCTION(Application::OnEvent));

		_context.init(_window->GetExtent().width, _window->GetExtent().height, static_cast<GLFWwindow*>(_window->GetNativeWindow()));

		VulkanRenderer::init(&_context, _window);

		_vulkanImGUI = new VulkanImGUI(static_cast<GLFWwindow*>(_window->GetNativeWindow()), _context);
		_vulkanImGUI->initImGUI();

	}

	Application::~Application() {}

	

	void Application::Run()
	{

		while (running)
		{
			
			float time = (float)glfwGetTime();
			Timestep timestep = time - _lastFrameTime;
			_lastFrameTime = time;

			_imGuiLayer->Begin();
			for (Layer* layer : _layerStack)
				layer->OnImGuiRender();
			_imGuiLayer->End();

			for (Layer* layer : _layerStack)
				layer->Update(timestep);
			
			_window->Update();

		}



		cleanup(); 

	}

	

	void Application::cleanup()
	{
		vkDeviceWaitIdle(_context.getDevice());

		VulkanRenderer::cleanup();

		for (auto& layer : _layerStack)
			layer->OnDetach();
	}

	void Application::PushLayer(Layer* layer)
	{
		_layerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		_layerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNCTION(Application::OnWindowClose));

		for (auto i = _layerStack.end(); i != _layerStack.begin(); )
		{
			(*--i)->OnEvent(e);
			if (e.handled)
				break;
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		running = false;
		return true;
	}

	// temporary helper function, creates a 1x1x1 cube centered at offset
	
}