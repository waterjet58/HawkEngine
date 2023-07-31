#include <Hawk.h>
#include "imgui/imgui.h"


class ExampleLayer : public Hawk::Layer
{
public:
	ExampleLayer() : Layer("Example") {}

	void Update() override
	{

	}

	void OnEvent(Hawk::Event& event) override
	{
		HWK_TRACE("{0}", event.GetName());
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Test");
		ImGui::Text("Hello World");
		ImGui::End();
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