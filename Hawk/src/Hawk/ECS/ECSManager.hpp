#pragma once

#include "Components/ComponentManager.h"
#include "EntityManager.h"
#include "Systems/SystemManager.h"
#include "Entity.h"
#include <memory>

namespace Hawk {
	class ECSManager {
	public:
		void init()
		{
			_componentManager = std::make_unique<ComponentManager>();
			_entityManager = std::make_unique<EntityManager>();
			_systemManager = std::make_unique<SystemManager>();
		}

		Entity createEntity()
		{
			return _entityManager->createEntity();
		}

		void destroyEntity(Entity entity)
		{
			_entityManager->destroyEntity(entity);
			_componentManager->entityDestroyed(entity);
			_systemManager->entityDestroyed(entity);
		}

		template<typename T>
		void registerComponent()
		{
			_componentManager->registerComponent<T>();
		}

		template<typename T>
		void addComponent(Entity entity, T component)
		{
			_componentManager->AddComponent<T>(entity, component);

			auto signature = _entityManager->getSignature(entity);
			signature.set(_componentManager->getComponentType<T>(), true);
			_entityManager->setSignature(entity, signature);

			_systemManager->entitySignatureChanged(entity, signature);
		}

		template<typename T>
		void removeComponent(Entity entity)
		{
			_componentManager->RemoveComponent<T>(entity);

			auto signature = _entityManager->getSignature(entity);
			signature.set(_componentManager->getComponentType<T>(), true);
			_entityManager->setSignature(entity, signature);

			_systemManager->entitySignatureChanged(entity, signature);
		}

		template<typename T>
		T& getComponent(Entity entity)
		{
			return _componentManager->getComponent<T>(entity);
		}

		template<typename T>
		ComponentType getComponentType()
		{
			return _componentManager->getComponentType<T>();
		}

		template<typename T>
		std::shared_ptr<T> registerSystem()
		{
			return _systemManager->registerSystem<T>();
		}

		template<typename T>
		void setSystemSignature(Signature signature)
		{
			_systemManager->setSignature<T>(signature);
		}

	private:
		std::unique_ptr<ComponentManager> _componentManager;
		std::unique_ptr<EntityManager> _entityManager;
		std::unique_ptr<SystemManager> _systemManager;
	};
}