#include "hwkPrecompiledHeader.h"
#include "Application.h"
#include "Hawk/Events/Event.h"
#include "Hawk/Events/ApplicationEvent.h"
#include "Hawk/Core/Log.h"
//#include "ImGui/ImGUILayer.h"
//#include "glad/glad.h"
#include "Input.h"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#include "Hawk/ECS/Components/Sprite.h"
#include "Hawk/Math/Quaternion.h"
#include "Timestep.h"
#include <Hawk/ECS/Components/Mesh.h>
#include "Hawk/Core/KeyCodes.h"

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

		_model = createCubeModel(_context, {0.f,0.f,0.f});

		for (int i = 0; i < 20; i++)
		{
			Entity entity;
			entity = _ecsManager->createEntity();
			Mesh mesh;

			mesh.transform.position = { -1.f + (i * 1.f),0.f, 2.f};

			mesh.model = _model;
			mesh.transform.scale = { .75f, .75f, .75f };
			mesh.transform.rotation = {45.f, 45.f, 0.f};

			_ecsManager->addComponent<Mesh>(entity, mesh);
		}

	}

	Application::~Application() {}

	

	void Application::Run()
	{

		Camera camera{};
		float x = 0.f;
		float y = 0.f;
		float z = 0.f;

		float xRot = 0.f;
		float yRot = 0.f;
		float zRot = 0.f;

		while (running)
		{
			float aspect = _renderer.getAspectRatio();
			camera.setPerspectiveProjection(glm::radians(50.f), aspect, .1f, 100.f);
			camera.setViewYXZ({ x,y,z }, { xRot,yRot,zRot });


			float time = (float)glfwGetTime();
			Timestep timestep = time - _lastFrameTime;
			_lastFrameTime = time;

			if (Input::IsKeyPressed(HWK_KEY_D))
				x += timestep * 2.f;

			if (Input::IsKeyPressed(HWK_KEY_A))
				x -= timestep * 2.f;

			if (Input::IsKeyPressed(HWK_KEY_S))
				z -= timestep * 2.f;

			if (Input::IsKeyPressed(HWK_KEY_W))
				z += timestep * 2.f;

			if (Input::IsKeyPressed(HWK_KEY_LEFT_CONTROL))
				y += timestep * 2.f;

			if (Input::IsKeyPressed(HWK_KEY_LEFT_SHIFT))
				y -= timestep * 2.f;

			if (Input::IsKeyPressed(HWK_KEY_LEFT))
				yRot -= timestep * 2.f;

			if (Input::IsKeyPressed(HWK_KEY_RIGHT))
				yRot += timestep * 2.f;

			if (Input::IsKeyPressed(HWK_KEY_DOWN))
				xRot -= timestep * 2.f;

			if (Input::IsKeyPressed(HWK_KEY_UP))
				xRot += timestep * 2.f;

			if (Input::IsKeyPressed(HWK_KEY_Q))
				zRot -= timestep * 2.f;

			if (Input::IsKeyPressed(HWK_KEY_E))
				zRot += timestep * 2.f;

			for (Layer* layer : _layerStack)
				layer->Update(timestep);

			_imGuiLayer->Begin();
			for (Layer* layer : _layerStack)
				layer->OnImGuiRender();
			_imGuiLayer->End();

			
			_window->Update();

			if (auto commandBuffer = _renderer.beginFrame())
			{
				_renderer.beginSwapChainRenderPass(commandBuffer);

				//System updates
				meshRenderer->Update(timestep, commandBuffer, camera);


				//ImGUI draw Data
				ImDrawData* draw_data = ImGui::GetDrawData();
				ImGui_ImplVulkan_RenderDrawData(draw_data, commandBuffer);


				//End of RenderPass and Frame
				_renderer.endSwapChainRenderPass(commandBuffer);
				_renderer.endFrame();
			}
			
		}

		

		//vkDeviceWaitIdle(_context->getDevice());

		cleanup(); 

	}

	void Application::RegisterComponents()
	{
		//_ecsManager->registerComponent<Transform2D>();
		_ecsManager->registerComponent<Transform3D>();
		//_ecsManager->registerComponent<Sprite>();
		_ecsManager->registerComponent<Mesh>();
	}

	void Application::RegisterSystems()
	{
		//SPRITE RENDERER////////////////////////////////////////////////////////////////
		//spriteRenderer = _ecsManager->registerSystem<SpriteRendererSystem>();
		//{
		//	Signature signature;
		//	signature.set(_ecsManager->getComponentType<Sprite>());
		//	//signature.set(_ecsManager->getComponentType<Transform3D>());
		//	_ecsManager->setSystemSignature<SpriteRendererSystem>(signature);
		//}
		
		//spriteRenderer->Init(_ecsManager, &_context, _renderer.getSwapChainRenderPass());
		//SPRITE RENDERER////////////////////////////////////////////////////////////////
		

		//MESH RENDERER////////////////////////////////////////////////////////////////
		meshRenderer = _ecsManager->registerSystem<MeshRendererSystem>();
		{
			Signature signature;
			signature.set(_ecsManager->getComponentType<Mesh>());
			//signature.set(_ecsManager->getComponentType<Transform3D>());
			_ecsManager->setSystemSignature<MeshRendererSystem>(signature);
		}

		meshRenderer->Init(_ecsManager, &_context, _renderer.getSwapChainRenderPass());
		//MESH RENDERER////////////////////////////////////////////////////////////////


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

	// temporary helper function, creates a 1x1x1 cube centered at offset
	std::unique_ptr<Model> Application::createCubeModel(VulkanContext& device, glm::vec3 offset) {
		Model::Builder modelBuilder{};
		modelBuilder.vertices = {
			// left face (white)
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

			// right face (yellow)
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

			// top face (orange, remember y axis points down)
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

			// bottom face (red)
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

			// nose face (blue)
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

			// tail face (green)
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
		};
		for (auto& v : modelBuilder.vertices) {
			v.position += offset;
		}

		modelBuilder.indices = { 0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
								12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21 };

		return std::make_unique<Model>(device, modelBuilder);
	}
}