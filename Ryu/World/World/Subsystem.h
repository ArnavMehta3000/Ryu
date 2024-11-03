#pragma once
#include "Common/Common.h"
#include "Utils/Factory.h"

namespace Ryu::World
{
	class World;

	/**
	 * @brief Base clas for all world subsystems
	 */
	class Subsystem
		: public Utils::Factory<Subsystem, World*>
		, public ITickable<Subsystem>
	{
	public:
		Subsystem(Key) {}
		virtual ~Subsystem() = default;

		/**
		 * @brief Can subsystem be ticked
		 * @return true if subsystem can be ticked
		 */
		constexpr inline RYU_API bool CanTick() const { return m_canTick; }

		/**
		 * @brief Set if subsystem can be ticked
		 * @param canTick If subsystem can be ticked
		 */
		constexpr inline RYU_API void CanTick(bool canTick) { m_canTick = canTick; }

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

		inline constexpr RYU_API bool IsInitialized() const noexcept { return m_isInitialized; }
		virtual RYU_API std::string_view GetName() = 0;

	protected:
		virtual RYU_API bool OnInit() = 0;
		virtual RYU_API void OnShutdown() = 0;
		virtual RYU_API void OnTick(f64 dt) = 0;


	private:
		bool m_canTick{ true };
		bool m_isInitialized{ false };
	};
}

#define RYU_WORLD_SUBSYSTEM(ClassName) class ClassName : public ::Ryu::World::Subsystem::Registrar<ClassName>
