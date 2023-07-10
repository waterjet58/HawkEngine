#pragma once

#include "Hawk/Layer.h"

namespace Hawk {
	class ImGUILayer : public Layer
	{
	private:
		virtual void OnEvent(Event& e) {}
		virtual void Update() {}
	
	public:
		ImGUILayer();
		~ImGUILayer();
	};
}

