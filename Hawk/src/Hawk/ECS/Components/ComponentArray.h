#pragma once

#include <unordered_map>
#include "Hawk/ECS/Entity.h"
#include "Hawk/Core.h"

namespace Hawk {

	class IComponentArray {
	public:
		virtual ~IComponentArray() = default;
		virtual void EntityDestroyed(Entity entity) = 0;
	};

	template<typename T>
	class ComponentArray : public IComponentArray
	{
	public:
		//
		void InsertData(Entity entity, T component)
		{
			HWK_ASSERT(_EntityToIndexMap.find(entity) == _EntityToIndexMap.end() && "Component added to same entity more than once");

			// Put new entry at end and update the maps
			uint32_t newIndex = _Size;
			_EntityToIndexMap[entity] = newIndex;
			_IndexToEntityMap[newIndex] = entity;
			_ComponentArray[newIndex] = component;
			_Size++;
		}

		//
		void RemoveData(Entity entity)
		{
			HWK_ASSERT(_EntityToIndexMap.find(entity) != _EntityToIndexMap.end() && "Removing component that does not exist");

			// Copy element at end into deleted element's place to maintain density
			uint32_t indexOfRemovedEntity = _EntityToIndexMap[entity];
			uint32_t indexOfLastElement = _Size - 1;
			_ComponentArray[indexOfRemovedEntity] = _ComponentArray[indexOfLastElement];

			// Update map to point to moved spot
			Entity entityOfLastElement = _IndexToEntityMap[indexOfLastElement];
			_EntityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
			_IndexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

			_EntityToIndexMap.erase(entity);
			_IndexToEntityMap.erase(indexOfLastElement);

			_Size++;
		}

		//Get a reference to an entity's component array
		T& GetData(Entity entity)
		{
			HWK_ASSERT(_EntityToIndexMap.find(entity) != _EntityToIndexMap.end() && "Retrieving component that does not exist");

			// Return a reference to the entity's component
			return _ComponentArray[_EntityToIndexMap[entity]];
		}

		//Destroy an enitity
		void EntityDestroyed(Entity entity) override
		{
			if (_EntityToIndexMap.find(entity) != _EntityToIndexMap.end())
			{
				// Remove the entity's component if it existed
				RemoveData(entity);
			}
		}


	private:
		//Packed array of components of a generic type T
		//Set to a specificed max where each entity has a unique spot
		std::array<T, MAX_ENTITIES> _ComponentArray;

		// Map from an entity ID to an array index.
		std::unordered_map<Entity, uint32_t> _EntityToIndexMap;

		// Map from an array index to an entity ID.
		std::unordered_map<uint32_t, Entity> _IndexToEntityMap;

		// Total size of valid entries in the array.
		uint32_t _Size;
	};

}