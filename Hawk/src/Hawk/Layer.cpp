#include "hwkPrecompiledHeader.h"
#include "Layer.h"

namespace Hawk {
	Layer::Layer(const std::string& debugName)
	{
		_debugName = debugName;
	}

	Layer::~Layer() {}
}
