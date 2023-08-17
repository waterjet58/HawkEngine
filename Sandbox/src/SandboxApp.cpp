#include <Hawk.h>
#include "imgui/imgui.h"



class ExampleLayer : public Hawk::Layer
{

	struct UniformBufferObject
	{
		alignas(16) glm::mat4 projectionView{1.f};
		alignas(16) glm::vec3 lightDir = glm::normalize(glm::vec3{1.f, -3.f, -1.f});
	};

public:
	ExampleLayer() : Layer("Example") 
	{

		_context = Hawk::VulkanRenderer::GetContext();

		descriptorPool = Hawk::DescriptorPool::Builder(*_context)
			.setMaxSets(Hawk::VulkanSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Hawk::VulkanSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();

		descriptorSetLayout = Hawk::DescriptorSetLayout::Builder(*_context)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
			.build();

		

		for (int i = 0; i < uboBuffers.size(); i++)
		{
			uboBuffers[i] = std::make_unique<Hawk::BufferObject>(
				*_context,
				sizeof(UniformBufferObject),
				Hawk::VulkanSwapChain::MAX_FRAMES_IN_FLIGHT,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				_context->properties.limits.minUniformBufferOffsetAlignment);

			uboBuffers[i]->map();
		}

		for (int i = 0; i < globalDescriptorSets.size(); i++)
		{
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			Hawk::DescriptorWriter(*descriptorSetLayout, *descriptorPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}


		_ecsManager = std::make_shared<Hawk::ECSManager>(); //Need to make a static instance to get for the windowsWindow
		_ecsManager->init();
		RegisterComponents();
		RegisterSystems();


		_model = Hawk::Model::createModelFromFile(*_context, "C:\\EngineDev\\Hawk\\Hawk\\Models\\lowpolyhuman.gltf");
		//_model = createCubeModel(*Hawk::VulkanRenderer::GetContext(), { 0.f,0.f,0.f });

		for (int i = 0; i < 20; i++)
		{
			Hawk::Entity entity;
			entity = _ecsManager->createEntity();
			Hawk::Mesh mesh;

			mesh.transform.position = { 0.f + (i * 1.f) , .2f, 2.f };
			mesh.model = _model;
			mesh.transform.scale = { .55f, .55f, .55f };
			mesh.transform.rotation = { 180.f, 0.f + (90.f * i), 0.f };

			_ecsManager->addComponent<Hawk::Mesh>(entity, mesh);
		}




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
			int frameIndex = Hawk::VulkanRenderer::getFrameIndex();

			Hawk::FrameData frameData //Setup frame data
			{
				frameIndex,
				timestep,
				commandBuffer,
				camera,
				globalDescriptorSets[frameIndex]
			};

			//Update
			UniformBufferObject ubo{};
			ubo.projectionView = camera.getProjection() * camera.getView();
			uboBuffers[frameIndex]->writeToBuffer(&ubo);
			uboBuffers[frameIndex]->flush();
			
			


			//Render
			Hawk::VulkanRenderer::beginSwapChainRenderPass(commandBuffer);

			//System updates
			meshRenderer->Update(timestep, frameData);

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

		meshRenderer->Init(_ecsManager, Hawk::VulkanRenderer::GetContext(), Hawk::VulkanRenderer::getSwapChainRenderPass(), descriptorSetLayout->getDescriptorSetLayout());
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
	std::unique_ptr<Hawk::DescriptorPool> descriptorPool;
	std::vector<VkDescriptorSet> globalDescriptorSets{Hawk::VulkanSwapChain::MAX_FRAMES_IN_FLIGHT};
	std::vector<std::unique_ptr<Hawk::BufferObject>> uboBuffers{Hawk::VulkanSwapChain::MAX_FRAMES_IN_FLIGHT};
	std::unique_ptr<Hawk::DescriptorSetLayout> descriptorSetLayout;
	Hawk::VulkanContext* _context;
	

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


