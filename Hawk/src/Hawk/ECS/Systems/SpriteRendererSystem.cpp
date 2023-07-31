#include "hwkPrecompiledHeader.h"
#include "SpriteRendererSystem.h"
#include "Hawk/ECS/Components/Transform.h"


namespace Hawk {

	
	void SpriteRendererSystem::Init(std::shared_ptr<ECSManager> manager)
	{
		_manager = manager;
	}

	void SpriteRendererSystem::Update(float dt)
	{
		for (auto const& entity : _entities)
		{
			auto& transform = _manager->getComponent<Transform2D>(entity);

			//HWK_TRACE("Sprite Renderer updating");
		}
	}

}