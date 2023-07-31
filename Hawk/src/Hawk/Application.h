#pragma once

#include "Core.h"
#include "Window.h"
#include "Events/Event.h"
#include "Hawk/LayerStack.h"
#include "Events/ApplicationEvent.h"
#include "Hawk/ImGui/ImGUILayer.h"
#include "Renderer/GraphicsContext.h"
#include "ECS/ECSManager.hpp"
#include "ECS/Systems/SpriteRendererSystem.h"

namespace Hawk {

	class Application
	{
	private:
		std::unique_ptr<Window> _window;
		ImGUILayer* _imGuiLayer;
		VulkanContext* _context;
		std::shared_ptr<ECSManager> _ecsManager;
		bool running = true;
		LayerStack _layerStack;
		static Application* s_Instance;
		bool OnWindowClose(WindowCloseEvent& event);

		//SYSTEMS
		std::shared_ptr<SpriteRendererSystem> spriteRenderer;

		//SYSTEMS



		unsigned int _VertexArray, _VertexBuffer, _IndexBuffer;
	public:
		Application();
		virtual ~Application();

		double clockToMilliseconds(clock_t ticks);

		void Run();
		void cleanup();
		void OnEvent(Event& e);
		void RegisterSystems();
		void RegisterComponents();
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		static Application& Get() { return *s_Instance; }
		Window& GetWindow() { return *_window; }
		GraphicsContext& GetGraphicsContext() { return *_context; }
	};

	//To be defined in client
	Application* CreateApplication();
}


