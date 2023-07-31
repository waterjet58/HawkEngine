#pragma once
#include "System.h"
#include "Hawk/ECS/ECSManager.hpp"

namespace Hawk {
	
	class SpriteRendererSystem : public System
	{
	public:
		void Init(std::shared_ptr<ECSManager> manager);

		void Update(float dt);

	private:
		std::shared_ptr<ECSManager> _manager;
	};
	
}
