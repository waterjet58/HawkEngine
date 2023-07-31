#pragma once

#include <memory>
#include <unordered_map>
#include "System.h"

namespace Hawk {
	class SystemManager {
	public:
		template<typename T>
		std::shared_ptr<T> registerSystem()
		{
			const char* typeName = typeid(T).name();

			HWK_ASSERT(_systems.find(typeName) == _systems.end() "Registering a system more than once");

			auto system = std::make_shared<T>();
			_systems.insert({typeName, system});
			//Returning system so it can be used externally
			return system;
		}

		template<typename T>
		void setSignature(Signature signature)
		{
			const char* typeName = typeid(T).name();

			HWK_ASSERT(_systems.find(typeName) != _systems.end() && "System was used before registering");

			_signatures.insert({ typeName, signature });
		}

		void entityDestroyed(Entity entity)
		{
			for (auto const& pair : _systems)
			{
				auto const& system = pair.second;

				system->_entities.erase(entity);
			}
		}

		void entitySignatureChanged(Entity entity, Signature entitySignature)
		{
			for (auto const& pair : _systems)
			{
				auto const& type = pair.first;
				auto const& system = pair.second;
				auto const& systemSignature = _signatures[type];

				if ((entitySignature & systemSignature) == systemSignature)
				{
					system->_entities.insert(entity);
				}
				else
				{
					system->_entities.erase(entity);
				}
			}
		}


	private:

		std::unordered_map<const char*, Signature> _signatures{};

		std::unordered_map<const char*, std::shared_ptr<System>> _systems;
	};
}