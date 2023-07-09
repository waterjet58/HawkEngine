#include <Hawk.h>

class ExampleLayer : public Hawk::Layer
{
public:
	ExampleLayer() : Layer("Example") {}

	void Update() override
	{

	}

	void OnEvent(Hawk::Event& event) override
	{
		HWK_TRACE("{0}", event.ToString());
	}
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