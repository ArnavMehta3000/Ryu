#pragma once
#include "World/Entity.h"
#include "World/IWorldSubsystem.h"
#include "Logger/LogCategory.h"
#include "Event/Event.h"
#include <vector>
#include <entt/entity/registry.hpp>

namespace Ryu::World
{
	/**
	 * @brief Class representing a collection of entities, components and subsystems
	 */
	class World final
	{
		RYU_LOG_CATEGORY(World);
		RYU_DISABLE_COPY_AND_MOVE(World);
		friend class IWorldSubsystem;
	
	public:
		World() = default;
		~World() = default;

		/**
		 * @brief Initializes the world
		 */
		void Init();

		/**
		 * @brief Shuts down the world
		 */
		void Shutdown();

		/**
		 * @brief Updates the world
		 * @param dt 
		 */
		void Tick(f64 dt);

		/**
		 * @brief Creates a new entity
		 * @return Handle to the new entity
		 */
		RYU_API EntityHandle CreateEntity();

		/**
		 * @brief Destroys an entity
		 * @details Marks the entity for deletion, it will be destroyed at the end of the frame
		 * @param entity Handle to the entity to destroy
		 */
		RYU_API void DestroyEntity(const EntityHandle& entity);

		/**
		 * @brief Gets the number of alive entities
		 * @return Number of alive entities
		 */
		RYU_API u64 GetAliveEntityCount() const;

		/**
		 * @brief Registers a new subsystem
		 * @tparam T The type of the subsystem
		 * @tparam Args The types of the arguments
		 */
		template <typename T> requires IsDerived<IWorldSubsystem, T>
		RYU_API void RegisterSubsystem(bool initSubsystem = false)
		{
			std::unique_ptr<IWorldSubsystem> ptr(new T(this));
			if (initSubsystem) { ptr->Init(); }
			m_subsystems.push_back(std::move(ptr));
		}

		/**
		 * @brief Gets a registered subsystem
		 * @tparam T The type of the subsystem
		 * @return A pointer to the subsystem
		 */
		template <typename T>
		RYU_API T* GetSubsystem() const
		{
			for (auto& system : m_subsystems)
			{
				if (dynamic_cast<T*>(system.get()))
				{
					return dynamic_cast<T*>(system.get());
				}
			}
			return nullptr;
		}

		/**
		 * @brief Removes a registered subsystem
		 * @tparam T The type of the subsystem
		 */
		template <typename T>
		RYU_API void RemoveSubsystem()
		{
			for (auto& system : m_subsystems)
			{
				if (dynamic_cast<T*>(system.get()))
				{
					system->Shutdown();
					m_subsystems.erase(std::remove(m_subsystems.begin(), m_subsystems.end(), system), m_subsystems.end());
					break;
				}
			}
		}

		RYU_DECLARE_EVENT(OnEntityCreate, EntityHandle&);

	private:
		entt::registry                                m_registry;
		std::vector<std::unique_ptr<IWorldSubsystem>> m_subsystems;
		std::vector<Entity>                           m_entitiesMarkedForDeletion;
	};
}
