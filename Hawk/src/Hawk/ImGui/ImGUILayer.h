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

	public:
		ImGUILayer();
		~ImGUILayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();
	};
}

