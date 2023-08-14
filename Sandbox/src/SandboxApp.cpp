#include <Hawk.h>
#include "imgui/imgui.h"



class ExampleLayer : public Hawk::Layer
{
public:
	ExampleLayer() : Layer("Example") 
	{
		_ecsManager = std::make_shared<Hawk::ECSManager>(); //Need to make a static instance to get for the windowsWindow
		_ecsManager->init();
		RegisterComponents();
		RegisterSystems();

		_model = Hawk::Model::createModelFromFile(*Hawk::VulkanRenderer::GetContext(), "C:\\EngineDev\\Hawk\\Hawk\\Models\\rock.gltf");
		//_model = createCubeModel(*Hawk::VulkanRenderer::GetContext(), { 0.f,0.f,0.f });

		Hawk::Entity entity;
		entity = _ecsManager->createEntity();
		Hawk::Mesh mesh;

		mesh.transform.position = { 0.f ,0.f, 2.f };

		mesh.model = _model;
		mesh.transform.scale = { 1.f, 1.f, 1.f }; //REMEMBER TO UNCHECK THE +Y UP WHEN EXPORTING TO GLTF FORM BECAUSE VULKAN RENDERS -Y UP
		mesh.transform.rotation = { 0.f, 0.f, 180.f };

		_ecsManager->addComponent<Hawk::Mesh>(entity, mesh);

	}

	void Update(Hawk::Timestep timestep) override
	{
		frames++;

		float aspect = Hawk::VulkanRenderer::getAspectRatio();
		camera.setPerspectiveProjection(glm::radians(70.f), aspect, .1f, 100.f);
		camera.setViewYXZ({ x,y,z }, { xRot,yRot,zRot });

		cameraMovement(timestep);

		if (auto commandBuffer = Hawk::VulkanRenderer::beginFrame())
		{
			Hawk::VulkanRenderer::beginSwapChainRenderPass(commandBuffer);

			//System updates
			meshRenderer->Update(timestep, commandBuffer, camera);

			//ImGUI draw Data
			ImDrawData* draw_data = ImGui::GetDrawData();
			ImGui_ImplVulkan_RenderDrawData(draw_data, commandBuffer);


			//End of RenderPass and Frame
			Hawk::VulkanRenderer::endSwapChainRenderPass(commandBuffer);
			Hawk::VulkanRenderer::endFrame();
		}

		if ((totalTime += timestep) > 1.f)
		{
			totalTime = 0.f;
			frameRate =(float)frames * .5f + frameRate * .5f;
			frames = 0;
		}
	}

	void OnEvent(Hawk::Event& event) override
	{
		//HWK_TRACE("{0}", event.GetName());
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Test");
		char label[50];
		strcpy(label, "FPS: ");
		strcat(label, " %.3f");
		ImGui::Text(label, frameRate);
		ImGui::End();
	}

	void RegisterComponents()
	{
		_ecsManager->registerComponent<Hawk::Transform3D>();
		_ecsManager->registerComponent<Hawk::Mesh>();
	}

	void cameraMovement(Hawk::Timestep timestep)
	{
		if (Hawk::Input::IsKeyPressed(HWK_KEY_D))
			x += timestep * 2.f;

		if (Hawk::Input::IsKeyPressed(HWK_KEY_A))
			x -= timestep * 2.f;

		if (Hawk::Input::IsKeyPressed(HWK_KEY_S))
			z -= timestep * 2.f;

		if (Hawk::Input::IsKeyPressed(HWK_KEY_W))
			z += timestep * 2.f;

		if (Hawk::Input::IsKeyPressed(HWK_KEY_LEFT_CONTROL))
			y += timestep * 2.f;

		if (Hawk::Input::IsKeyPressed(HWK_KEY_LEFT_SHIFT))
			y -= timestep * 2.f;

		if (Hawk::Input::IsKeyPressed(HWK_KEY_LEFT))
			yRot -= timestep * 2.f;

		if (Hawk::Input::IsKeyPressed(HWK_KEY_RIGHT))
			yRot += timestep * 2.f;

		if (Hawk::Input::IsKeyPressed(HWK_KEY_DOWN))
			xRot -= timestep * 2.f;

		if (Hawk::Input::IsKeyPressed(HWK_KEY_UP))
			xRot += timestep * 2.f;

		if (Hawk::Input::IsKeyPressed(HWK_KEY_Q))
			zRot -= timestep * 2.f;

		if (Hawk::Input::IsKeyPressed(HWK_KEY_E))
			zRot += timestep * 2.f;
	}

	void RegisterSystems()
	{


		//MESH RENDERER////////////////////////////////////////////////////////////////
		meshRenderer = _ecsManager->registerSystem<Hawk::MeshRendererSystem>();
		{
			Hawk::Signature signature;
			signature.set(_ecsManager->getComponentType<Hawk::Mesh>());
			//signature.set(_ecsManager->getComponentType<Transform3D>());
			_ecsManager->setSystemSignature<Hawk::MeshRendererSystem>(signature);
		}

		meshRenderer->Init(_ecsManager, Hawk::VulkanRenderer::GetContext(), Hawk::VulkanRenderer::getSwapChainRenderPass());
		//MESH RENDERER////////////////////////////////////////////////////////////////


	}

	std::unique_ptr<Hawk::Model> createCubeModel(Hawk::VulkanContext& device, glm::vec3 offset) {
		Hawk::Model::Builder modelBuilder{};
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

		return std::make_unique<Hawk::Model>(device, modelBuilder);
	}

private:
	std::shared_ptr<Hawk::ECSManager> _ecsManager;
	std::shared_ptr<Hawk::Model> _model;
	std::shared_ptr<Hawk::MeshRendererSystem> meshRenderer;

	float totalTime = 0.f;
	float frameRate = 60.f;
	int frames = 0;
	Hawk::Camera camera{};
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;

	float xRot = 0.f;
	float yRot = 0.f;
	float zRot = 0.f;
};

class Sandbox : public Hawk::Application
{
public: 
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox()
	{

	}
};

Hawk::Application* Hawk::CreateApplication()
{
	return new Sandbox();
}


