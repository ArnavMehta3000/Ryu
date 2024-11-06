#pragma once
#include "World/Entity.h"
#include "World/Subsystem.h"
#include "Logger/LogCategory.h"
#include "Event/Event.h"
#include <vector>

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

		RYU_DECLARE_EVENT(OnEntityCreate, EntityHandle&);

	private:
		entt::registry                          m_registry;
		std::vector<std::unique_ptr<Subsystem>> m_subsystems;
		std::vector<Entity>                     m_entitiesMarkedForDeletion;
	};
}
