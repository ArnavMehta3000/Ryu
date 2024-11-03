#include "Subsystem.h"
#include "World.h"

namespace Ryu::World
{
	RYU_API bool Subsystem::Init()
	{
		m_isInitialized = OnInit();
		return m_isInitialized;
	}

	RYU_API void Subsystem::Shutdown()
	{
		OnShutdown();
		m_isInitialized = false;
	}

	RYU_API void Subsystem::Tick(f64 dt)
	{
		if (m_canTick)
		{
			OnTick(dt);
		}
	}


	RYU_WORLD_SUBSYSTEM(ExampleSubsystem)
	{
	public:
		static std::string_view GetStaticName() { return "ExampleSubsystem"; }

		ExampleSubsystem(World* world)
			: m_world(world)
		{
			// Make this subsystem non-tickable
			CanTick(false);
		}

		bool OnInit() override
		{
			return true;
		}

		void OnShutdown() override
		{
		}

		void OnTick(MAYBE_UNUSED f64 dt) override
		{
		}

	private:
		std::string_view GetName() override
		{
			return GetStaticName();
		}

	private:
		World* m_world;
	};

	
}
