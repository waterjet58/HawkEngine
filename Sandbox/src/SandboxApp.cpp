#include <Hawk.h>

class Sandbox : public Hawk::Application
{
public: 
	Sandbox()
	{

	}

	~Sandbox()
	{

	}
};

Hawk::Application* Hawk::CreateApplication()
{
	return new Sandbox();
}