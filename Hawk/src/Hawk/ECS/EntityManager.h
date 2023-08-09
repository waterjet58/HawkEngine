#pragma once

#include "Hawk/Core/Core.h"
#include <array>
#include <queue>
#include "Entity.h"

namespace Hawk {

	class EntityManager {
	public:
		EntityManager() 
		{
			//Fill up all of the available entities list with all possible ids
			for (Entity entity = 0; entity < MAX_ENTITIES; entity++)
			{
				_availableEntities.push(entity);
			}
		}

		Entity createEntity()
		{
			HWK_ASSERT(_activeEntityCount < MAX_ENTITIES && "Too many active entities");
			
			//Get an id from the front of the queue and pop it
			Entity id = _availableEntities.front();
			_availableEntities.pop();
			_activeEntityCount++;

			return id;
		}

		void destroyEntity(Entity entity)
		{
			HWK_ASSERT(entity < MAX_ENTITIES && "Entity out of range");

			//Reset the signatures at the entity's then push the now available entity
			//to the back of the available queue
			_signatures[entity].reset();
			_availableEntities.push(entity);
			_activeEntityCount--;
		}

		void setSignature(Entity entity, Signature signature)
		{
			HWK_ASSERT(entity < MAX_ENTITIES && "Entity out of range");

			_signatures[entity] = signature;
		}
		
		Signature getSignature(Entity entity)
		{
			HWK_ASSERT(entity < MAX_ENTITIES && "Entity out of range");

			return _signatures[entity];
		}

	private:
		std::queue<Entity> _availableEntities{};
		std::array<Signature, MAX_ENTITIES> _signatures{};
		uint32_t _activeEntityCount{};
	};


}