#include "hwkPrecompiledHeader.h"

#include "Application.h"

#include "Hawk/Events/ApplicationEvent.h"
#include "Hawk/Log.h"

namespace Hawk {
	Application::Application()
	{
		_window = std::unique_ptr<Window>(Window::Create());
	}

	Application::~Application()
	{

	}

	void Application::Run()
	{
		while (running)
		{
			
			_window->Update();
		}
	}
}