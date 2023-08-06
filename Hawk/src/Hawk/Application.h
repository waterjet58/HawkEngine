#pragma once

#include "Core.h"
#include "Window.h"
#include "Events/Event.h"
#include "Hawk/LayerStack.h"
#include "Events/ApplicationEvent.h"
#include "Hawk/ImGui/ImGUILayer.h"
#include "Renderer/GraphicsContext.h"
#include "ECS/Systems/SpriteRendererSystem.h"
#include "Platform/Vulkan/VulkanImGUI.h"
#include "Renderer/Model.h"

namespace Hawk {
	
	class Application
	{
	private:
		VulkanContext _context{};
		Window* _window{ Window::Create("HwkEngine", 1280, 920, _context) };
		VulkanRenderer _renderer{ _context, _window };
		ImGUILayer* _imGuiLayer;
		VulkanImGUI _vulkanImGUI{ static_cast<GLFWwindow*>(_window->GetNativeWindow()), _context, _renderer };
		std::shared_ptr<ECSManager> _ecsManager;
		std::shared_ptr<Model> _model;
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
		VulkanContext& GetGraphicsContext() { return _context; }
		std::shared_ptr<SpriteRendererSystem> getSpriteRenderer() { return spriteRenderer; }
		std::shared_ptr<ECSManager> getECSMananger() {	return _ecsManager; }
	};

	//To be defined in client
	Application* CreateApplication();
}


