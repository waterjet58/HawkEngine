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

		uint32_t extensions_count = 0;
		const char** extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
		
		_context = new VulkanContext(static_cast<GLFWwindow*>(Application::GetWindow().GetNativeWindow()));
		_context->Init();

		_imGuiLayer = new ImGUILayer();
		
		PushOverlay(_imGuiLayer);

		float vetices[3 * 3] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.0f,  0.5f, 0.0f,
		};


	}

	Application::~Application() {}

	void Application::Run()
	{
		
		ImGui_ImplVulkanH_Window* wd = _context->GetWindowData();
		ImVec4 clear_color = ImVec4(0.2f, 0.2f, 0.2f, 0.2f);
		ImGuiIO& io = ImGui::GetIO();

		while (running)
		{

			for (Layer* layer : _layerStack)
				layer->Update();

			_imGuiLayer->Begin(_context);
			for (Layer* layer : _layerStack)
				layer->OnImGuiRender();
			_imGuiLayer->End(_context);

			ImDrawData* main_draw_data = ImGui::GetDrawData();
			const bool main_is_minimized = (main_draw_data->DisplaySize.x <= 0.0f || main_draw_data->DisplaySize.y <= 0.0f);
			
			wd->ClearValue.color.float32[0] = clear_color.x * clear_color.w;
			wd->ClearValue.color.float32[1] = clear_color.y * clear_color.w;
			wd->ClearValue.color.float32[2] = clear_color.z * clear_color.w;
			wd->ClearValue.color.float32[3] = clear_color.w;
			
			if (!main_is_minimized)
				_context->FrameRender(wd, main_draw_data);

			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
			}

			// Present Main Platform Window
			if (!main_is_minimized)
				_context->FramePresent(wd);

			_window->Update();
		}

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