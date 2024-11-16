#pragma once
#include "Common/Common.h"
#include <entt/fwd.hpp>

namespace Ryu::Game
{
	class World;

	/**
	 * @brief World subsystem interface
	 * @details Acts like an accessor interface to the owning world and its registry
	 */
	class IWorldSubsystem
	{
	public:
		IWorldSubsystem(World* world);
		virtual ~IWorldSubsystem() = default;
		
		/**
		 * @brief Get a pointer to the owning world
		 * @return Pointer to the owning world
		 */
		inline constexpr RYU_API World* GetWorld() const noexcept;

		/**
		 * @brief Can subsystem be ticked
		 * @return true if subsystem can be ticked
		 */
		constexpr inline RYU_API bool CanTick() const noexcept;

		/**
		 * @brief Set if subsystem can be ticked
		 * @param canTick If subsystem can be ticked
		 */
		constexpr inline RYU_API void CanTick(bool canTick) noexcept;

		/**
		 * @brief Initialize subsystem
		 * @details Calls OnInit
		 * @return true if initialization was successful
		 */
		RYU_API bool Init();

		/**
		 * @brief Shutdown subsystem
		 * @details Calls OnShutdown
		 */
		RYU_API void Shutdown();

		/**
		 * @brief Tick subsystem
		 * @param dt Delta time
		 * @details Calls OnTick if subsystem can be ticked
		 */
		RYU_API void Tick(f64 dt);

		/**
		 * @brief Check if subsystem is initialized
		 * @return true if subsystem is initialized
		 */
		inline constexpr RYU_API bool IsInitialized() const noexcept { return m_isInitialized; }

		/**
		 * @brief Get the name of the subsystem
		 * @return Name of the subsystem
		 */
		virtual RYU_API std::string_view GetName() = 0;
	protected:
		inline void SetWorld(World* world) noexcept { m_world = world; }
		entt::registry& GetRegistry() const noexcept;  // Implemented in World/Subsystem.cpp

		virtual RYU_API bool OnInit() = 0;
		virtual RYU_API void OnShutdown() = 0;
		virtual RYU_API void OnTick(f64 dt) = 0;
	private:
		World* m_world;
		bool   m_canTick;
		bool   m_isInitialized;
	};
}
