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

		_ecsManager = std::make_shared<ECSManager>();
		_ecsManager->init();
		RegisterComponents();
		RegisterSystems();
		std::vector<Entity> entities(MAX_ENTITIES - 1);
		
		//_imGuiLayer = new ImGUILayer();
		//PushOverlay(_imGuiLayer);

		for (auto& entity : entities)
		{
			entity = _ecsManager->createEntity();
			_ecsManager->addComponent<Transform2D>(entity, Transform2D{});
		}

	}

	Application::~Application() {}

	

	void Application::Run()
	{

		clock_t deltaTime = 0;
		unsigned int frames = 0;
		double frameRate = 30;

		while (running)
		{
			clock_t beginFrame = clock();

			for (Layer* layer : _layerStack)
				layer->Update();

			_imGuiLayer->Begin();
			for (Layer* layer : _layerStack)
				layer->OnImGuiRender();
			_imGuiLayer->End();

			
			_window->Update();

			//spriteRenderer->Update(0.0f);

			clock_t endFrame = clock();

			deltaTime += endFrame - beginFrame;
			frames++;

			if (clockToMilliseconds(deltaTime) > 1000.0) { //every second
				frameRate = (double)frames * 0.5 + frameRate * 0.5; //more stable
				HWK_INFO("FrameRate: {0}", frameRate);
				frames = 0;
				deltaTime -= CLOCKS_PER_SEC;
			}


		}

		

		//vkDeviceWaitIdle(_context->getDevice());

		cleanup(); 

	}

	double Application::clockToMilliseconds(clock_t ticks) {
		// units/(units/time) => time (seconds) * 1000 = milliseconds
		return (ticks / (double)CLOCKS_PER_SEC) * 1000.0;
	}

	void Application::RegisterComponents()
	{
		_ecsManager->registerComponent<Transform2D>();
		_ecsManager->registerComponent<Transform3D>();
	}

	void Application::RegisterSystems()
	{
		spriteRenderer = _ecsManager->registerSystem<SpriteRendererSystem>();
		{
			Signature signature;
			signature.set(_ecsManager->getComponentType<Transform2D>());
			//signature.set(_ecsManager->getComponentType<Transform3D>());
			_ecsManager->setSystemSignature<SpriteRendererSystem>(signature);
		}

		spriteRenderer->Init(_ecsManager);
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