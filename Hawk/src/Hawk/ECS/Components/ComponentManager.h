#pragma once

#include "Hawk/ECS/Entity.h"
#include <unordered_map>
#include <any>
#include <memory>
#include "ComponentArray.h"

namespace Hawk {
	class ComponentManager
	{
	public:
		template<typename T>
		void registerComponent()
		{
			const char* typeName = typeid(T).name();

			HWK_ASSERT(_componentTypes.find(typeName) == _componentTypes.end() && "Registering component type more than once");

			//Add this componet to the component type map
			_componentTypes.insert({ typeName, _nextComponentType });

			//Create a component array pointer and add it to the component array map
			_componentArrays.insert({ typeName, std::make_shared<ComponentArray<T>>() });

			_nextComponentType++;
		}

		template<typename T>
		ComponentType getComponentType()
		{
			const char* typeName = typeid(T).name();

			HWK_ASSERT(_componentTypes.find(typeName) != _componentTypes.end() && "Component not registered");

			return _componentTypes[typeName];
		}

		template<typename T>
		void AddComponent(Entity entity, T component)
		{
			// Add a component to the array for an entity
			getComponentArray<T>()->InsertData(entity, component);
		}

		template<typename T>
		void RemoveComponent(Entity entity)
		{
			// Remove a component from the array for an entity
			getComponentArray<T>()->RemoveData(entity);
		}

		template<typename T>
		T& getComponent(Entity entity)
		{
			// Get a reference to a component from the array for an entity
			return getComponentArray<T>()->GetData(entity);
		}

		void entityDestroyed(Entity entity)
		{
			// Notify each component array that an entity has been destroyed
			// If it has a component for that entity, it will remove it
			for (auto const& pair : _componentArrays)
			{
				auto const& component = pair.second;

				component->EntityDestroyed(entity);
			}
		}

	private:
		//Map from type string pointer to a component type
		std::unordered_map<const char*, ComponentType> _componentTypes{};

		//Map from type string pointer to a component array
		std::unordered_map<const char*, std::shared_ptr<IComponentArray>> _componentArrays{};

		ComponentType _nextComponentType{};

		//Helper function to get the statically casted pointer to the component array of type T
		template<typename T>
		std::shared_ptr<ComponentArray<T>> getComponentArray()
		{
			const char* typeName = typeid(T).name();

			HWK_ASSERT(_componentTypes.find(typeName) != _componentTypes.end() && "Component not registered before being used");

			return std::static_pointer_cast<ComponentArray<T>>(_componentArrays[typeName]);
		}

	};
}