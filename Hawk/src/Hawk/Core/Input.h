#pragma once

namespace Hawk
{
	class Input
	{
	private:
		static Input* _instance;

	public:

		static bool IsKeyPressed(int keycode) { return _instance->IsKeyPressedImpl(keycode); }
		static bool IsMouseButtonPressed(int button) { return _instance->IsMouseButtonPressedImpl(button); }
		static std::pair<float, float> GetMousePos() { return _instance->GetMousePosImpl(); }
		static bool GetMouseX() { return _instance->GetMouseXImpl(); }
		static bool GetMouseY() { return _instance->GetMouseYImpl(); }

	protected:

		virtual bool IsKeyPressedImpl(int keycode) = 0;
		virtual bool IsMouseButtonPressedImpl(int button) = 0;
		virtual std::pair<float, float> GetMousePosImpl() = 0;
		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;

	};


}