#pragma once

#ifdef HWK_PLATFORM_WINDOWS // If the application is being ran in windows

extern Hawk::Application* Hawk::CreateApplication(); //Externally create a Hawk Engine Application

int main(int argc, char** argv)
{
	Hawk::Log::Init();

	auto app = Hawk::CreateApplication();
	app->Run();
	delete app;
}

#endif