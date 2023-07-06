#pragma once

#include "Core.h"
#include "Window.h"
#include "Events/Event.h"

namespace Hawk {

	class Application
	{
		public:
			Application();
			virtual ~Application();

			void Run();
	private:
		std::unique_ptr<Window> _window;
		bool running = true;
	};

	//To be defined in client
	Application* CreateApplication();
}


