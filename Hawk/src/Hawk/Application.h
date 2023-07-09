#pragma once

#include "Core.h"
#include "Window.h"
#include "Events/Event.h"
#include "Hawk/LayerStack.h"
#include "Events/ApplicationEvent.h"

namespace Hawk {

	class Application
	{
	private:
		std::unique_ptr<Window> _window;
		bool running = true;
		LayerStack _layerStack;

		bool OnWindowClose(WindowCloseEvent& event);
	public:
		Application();
		virtual ~Application();

		void Run();
		void cleanup();
		void OnEvent(Event& e);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

	};

	//To be defined in client
	Application* CreateApplication();
}


