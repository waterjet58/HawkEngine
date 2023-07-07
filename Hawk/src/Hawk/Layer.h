#pragma once

#include "Hawk/Core.h"
#include "Hawk/Events/Event.h"

namespace Hawk 
{
	class Layer
	{
	protected:
		std::string _debugName;

	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnEvent(Event& e) {}
		virtual void Update() {}

		inline const std::string& GetName() const { return _debugName; }
	};
}


