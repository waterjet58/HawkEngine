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
#include "ECS/Components/Sprite.h"



namespace Hawk {

#define BIND_EVENT_FUNCTION(x) std::bind(&x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;
	

	Application::Application()
	{
		HWK_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		_window->SetEventCallback(BIND_EVENT_FUNCTION(Application::OnEvent));

		_context.init(_window->GetExtent().width, _window->GetExtent().height, static_cast<GLFWwindow*>(_window->GetNativeWindow()));

		_renderer.init();

		_vulkanImGUI = new VulkanImGUI(static_cast<GLFWwindow*>(_window->GetNativeWindow()), _context, _renderer);
		_vulkanImGUI->initImGUI();

		_ecsManager = std::make_shared<ECSManager>(); //Need to make a static instance to get for the windowsWindow
		_ecsManager->init();
		RegisterComponents();
		RegisterSystems();

		//_imGuiLayer = new ImGUILayer();
		//PushOverlay(_imGuiLayer);

		std::vector<Model::Vertex> vertices{
			{{0.0f, -0.1f}, { 1.0f, 0.0f, 0.0f }}, //Red vertice
			{ {0.1f, 0.1f}, { 0.0f, 1.0f, 0.0f } }, //Green vertice
			{ {-0.1f, 0.1f}, { 0.0f, 0.0f, 1.0f } } //Blue vertice
		};
		_model = std::make_shared<Model>(_context, vertices);

		for (int i = 0; i < 50; i++)
		{
			Entity entity;
			entity = _ecsManager->createEntity();
			Sprite sprite;

			sprite.color = { (float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX };

			sprite.model = _model;
			sprite.transform.scale = { ((float)rand() / RAND_MAX) * 4, ((float)rand() / RAND_MAX) * 4 };
			sprite.transform.rotation = ((float)rand() / RAND_MAX) * glm::two_pi<float>();

			_ecsManager->addComponent<Sprite>(entity, sprite);
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

			if (auto commandBuffer = _renderer.beginFrame())
			{
				_renderer.beginSwapChainRenderPass(commandBuffer);
				
				

				spriteRenderer->Update(0.0f, commandBuffer);

				ImDrawData* draw_data = ImGui::GetDrawData();
				ImGui_ImplVulkan_RenderDrawData(draw_data, commandBuffer);

				_renderer.endSwapChainRenderPass(commandBuffer);
				_renderer.endFrame();
			}

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
		_ecsManager->registerComponent<Sprite>();
	}

	void Application::RegisterSystems()
	{
		spriteRenderer = _ecsManager->registerSystem<SpriteRendererSystem>();
		{
			Signature signature;
			signature.set(_ecsManager->getComponentType<Sprite>());
			//signature.set(_ecsManager->getComponentType<Transform3D>());
			_ecsManager->setSystemSignature<SpriteRendererSystem>(signature);
		}

		spriteRenderer->Init(_ecsManager, &_context, _renderer.getSwapChainRenderPass());
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