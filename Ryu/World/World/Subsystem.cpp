#include "Subsystem.h"
#include "World.h"

namespace Ryu::World
{
	entt::registry& IWorldSubsystem::GetRegistry() const noexcept
	{
		return GetWorld()->m_registry;
	}

	bool Subsystem::Init()
	{
		m_isInitialized = OnInit();
		return m_isInitialized;
	}

	void Subsystem::Shutdown()
	{
		OnShutdown();
		m_isInitialized = false;
	}

	void Subsystem::Tick(f64 dt)
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
		{
			// Make this subsystem non-tickable
			CanTick(false);
			SetWorld(world);
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
	};

	RYU_WORLD_SUBSYSTEM(MyNewSubsystem)
	{
	public:
		static std::string_view GetStaticName() { return "MyNewSubsystem"; }

		MyNewSubsystem(World* world)
		{
			// Make this subsystem non-tickable
			CanTick(false);
			SetWorld(world);
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
	};

	
}
