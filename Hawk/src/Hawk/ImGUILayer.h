#pragma once

#include "Hawk/Layer.h"
#include "Hawk/Events/MouseEvent.h"
#include "Hawk/Events/KeyEvent.h"
#include "Hawk/Events/ApplicationEvent.h"



namespace Hawk {
	class ImGUILayer : public Layer
	{
	private:
		float _time = 0.0;

		bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
		bool OnMouseScrolledEvent(MouseScrolledEvent& e);
		bool OnMouseMovedEvent(MouseMovedEvent& e);
		bool OnKeyPressedEvent(KeyPressedEvent& e);
		bool OnKeyReleasedEvent(KeyReleasedEvent& e);
		bool OnKeyTypedEvent(KeyTypedEvent& e);
		bool OnWindowResizeEvent(WindowResizeEvent& e);

	public:
		ImGUILayer();
		~ImGUILayer();

		virtual void OnEvent(Event& e) override;
		virtual void Update() override;
		virtual void OnAttach() override;
		virtual void OnDetach() override;
	};
}

