#pragma once

#include "Core.h"
#include "Window.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"

namespace Hawk {

	class Application
	{
		public:
			Application();
			virtual ~Application();

			void Run();
			void OnEvent(Event& e);
	private:
		std::unique_ptr<Window> _window;
		bool running = true;
	};

	//To be defined in client
	Application* CreateApplication();
}


