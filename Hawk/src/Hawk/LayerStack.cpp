#include "hwkPrecompiledHeader.h"
#include "LayerStack.h"

namespace Hawk {

	LayerStack::LayerStack()
	{
		_layerInsert = _layers.begin();
	}

	LayerStack::~LayerStack()
	{
		for (Layer* layer : _layers)
		{
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		_layerInsert = _layers.emplace(_layerInsert, layer);
	}

	void LayerStack::PushOverlay(Layer* overlay)
	{
		_layers.emplace_back(overlay);
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		auto i = std::find(_layers.begin(), _layers.end(), layer);
		if (i != _layers.end())
		{
			_layers.erase(i);
			_layerInsert--;
		}
	}

	void LayerStack::PopOverlay(Layer* overlay)
	{
		auto i = std::find(_layers.begin(), _layers.end(), overlay);
		if (i != _layers.end())
		{
			_layers.erase(i);
		}
	}

}