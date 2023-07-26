#include "hwkPrecompiledHeader.h"
#include "Application.h"
#include "Hawk/Events/Event.h"
#include "Hawk/Events/ApplicationEvent.h"
#include "Hawk/Log.h"
//#include "ImGui/ImGUILayer.h"
//#include "glad/glad.h"
#include "Input.h"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#include "Platform/Vulkan/VulkanContext.h"

namespace Hawk {

#define BIND_EVENT_FUNCTION(x) std::bind(&x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		HWK_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		WindowProperties props;
		props.Height = 980;
		props.Width = 1280;

		_window = std::unique_ptr<Window>(Window::Create(props));
		_window->SetEventCallback(BIND_EVENT_FUNCTION(Application::OnEvent));

		//_imGuiLayer = new ImGUILayer();
		//PushOverlay(_imGuiLayer);


	}

	Application::~Application() {}

	void Application::Run()
	{

		while (running)
		{

			for (Layer* layer : _layerStack)
				layer->Update();

			/*_imGuiLayer->Begin();
			for (Layer* layer : _layerStack)
				layer->OnImGuiRender();
			_imGuiLayer->End();*/

			

			_window->Update();
		}

		vkDeviceWaitIdle(_context->getDevice());

		cleanup(); 

	}

	void Application::cleanup()
	{
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
}