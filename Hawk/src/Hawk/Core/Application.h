#pragma once

#include "Core.h"
#include "Window.h"
#include "Hawk/Events/Event.h"
#include "Hawk/Core/LayerStack.h"
#include "Hawk/Events/ApplicationEvent.h"
#include "Hawk/ImGui/ImGUILayer.h"
#include "Hawk/Renderer/GraphicsContext.h"
#include "Hawk/ECS/Systems/SpriteRendererSystem.h"
#include <Hawk/ECS/Systems/MeshRendererSystem.h>
#include "Platform/Vulkan/VulkanImGUI.h"
#include "Hawk/Renderer/Model.h"
#include "Hawk/Core/Camera.h"

namespace Hawk {
	
	class Application
	{
	private:
		VulkanContext _context{};
		Window* _window{ Window::Create("HwkEngine", 1280, 920, _context) };
		VulkanRenderer _renderer{ _context, _window };
		ImGUILayer* _imGuiLayer;
		VulkanImGUI* _vulkanImGUI;
		std::shared_ptr<ECSManager> _ecsManager;
		std::shared_ptr<Model> _model;
		bool running = true;
		LayerStack _layerStack;
		static Application* s_Instance;
		bool OnWindowClose(WindowCloseEvent& event);
		std::unique_ptr<Model> createCubeModel(VulkanContext& device, glm::vec3 offset);
		float _lastFrameTime = 0.0f;
		float totalTime = 0.f;
		//SYSTEMS
		std::shared_ptr<SpriteRendererSystem> spriteRenderer;
		std::shared_ptr<MeshRendererSystem> meshRenderer;
		//SYSTEMS



		unsigned int _VertexArray, _VertexBuffer, _IndexBuffer;
	public:
		Application();
		virtual ~Application();

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
		VulkanRenderer& getRenderer() { return _renderer; }

	};

	//To be defined in client
	Application* CreateApplication();
}


