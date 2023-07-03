#pragma once

#ifdef HWK_PLATFORM_WINDOWS // If the application is being ran in windows

extern Hawk::Application* Hawk::CreateApplication(); //Externally create a Hawk Engine Application

int main(int argc, char** argv)
{
	Hawk::Log::Init();
	HWK_CORE_WARN("Initialized Log!");
	HWK_INFO("Hello Client here!");
	auto app = Hawk::CreateApplication();
	app->Run();
	delete app;
}

#endif