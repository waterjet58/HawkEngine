#pragma once

#include "Hawk/Core.h"
#include "Layer.h"

#include <vector>

namespace Hawk {
	class LayerStack
	{
	private:
		std::vector<Layer*> _layers;
		std::vector<Layer*>::iterator _layerInsert;

	public:
		LayerStack();
		~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);

		std::vector<Layer*>::iterator begin() { return _layers.begin(); }
		std::vector<Layer*>::iterator end() { return _layers.end(); }
	};
}

