#pragma once

#include "Hawk/ECS/Entity.h"
#include <set>

namespace Hawk {
	class System
	{
	public:
		std::set<Entity> _entities;
	};
}