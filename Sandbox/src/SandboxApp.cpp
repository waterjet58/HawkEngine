#include <Hawk.h>
#include "imgui/imgui.h"



class ExampleLayer : public Hawk::Layer
{
public:
	ExampleLayer() : Layer("Example") {}

	void Update(Hawk::Timestep timestep) override
	{
		frames++;
		

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

private:
	float totalTime = 0.f;
	float frameRate = 60.f;
	int frames = 0;
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


