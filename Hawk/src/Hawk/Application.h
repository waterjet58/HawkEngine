#pragma once

#include "Core.h"
#include "Window.h"
#include "Events/Event.h"
#include "Hawk/LayerStack.h"
#include "Events/ApplicationEvent.h"
#include "Hawk/ImGui/ImGUILayer.h"
#include "Renderer/GraphicsContext.h"

namespace Hawk {

	class Application
	{
	private:
		std::unique_ptr<Window> _window;
		ImGUILayer* _imGuiLayer;
		VulkanContext* _context;
		bool running = true;
		LayerStack _layerStack;
		static Application* s_Instance;
		bool OnWindowClose(WindowCloseEvent& event);

		unsigned int _VertexArray, _VertexBuffer, _IndexBuffer;
	public:
		Application();
		virtual ~Application();

		void Run();
		void cleanup();
		void OnEvent(Event& e);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		static Application& Get() { return *s_Instance; }
		Window& GetWindow() { return *_window; }
		GraphicsContext& GetGraphicsContext() { return *_context; }
	};

	//To be defined in client
	Application* CreateApplication();
}


