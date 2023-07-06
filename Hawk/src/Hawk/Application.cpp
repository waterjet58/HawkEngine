#include "hwkPrecompiledHeader.h"

#include "Application.h"
#include "Hawk/Log.h"

#include "Hawk/Events/ApplicationEvent.h"
#include "Hawk/Log.h"

namespace Hawk {

#define BIND_EVENT_FUNCTION(x) std::bind(&x, this, std::placeholders::_1)

	Application::Application()
	{
		_window = std::unique_ptr<Window>(Window::Create());
		_window->SetEventCallback(BIND_EVENT_FUNCTION(Application::OnEvent));
	}

	Application::~Application()
	{
		
	}

	void Application::OnEvent(Event& e)
	{
		HWK_CORE_INFO("{0}", e.ToString());
	}

	void Application::Run()
	{
		while (running)
		{
			_window->Update();
		}
	}
}