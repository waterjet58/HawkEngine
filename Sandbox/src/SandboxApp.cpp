#include <Hawk.h>

class ExampleLayer : public Hawk::Layer
{
public:
	ExampleLayer() : Layer("Example") {}

	void Update() override
	{
		//if (Hawk::Input::IsKeyPressed())
			//HWK_INFO("W is pressed");
	}

	void OnEvent(Hawk::Event& event) override
	{
		
	}
};

class Sandbox : public Hawk::Application
{
public: 
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		PushOverlay(new Hawk::ImGUILayer());
	}

	~Sandbox()
	{

	}
};

Hawk::Application* Hawk::CreateApplication()
{
	return new Sandbox();
}