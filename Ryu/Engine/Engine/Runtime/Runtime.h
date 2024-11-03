#pragma once
#include "App/Application.h"
#include "Logger/LogCategory.h"
#include "World/World.h"

namespace Ryu::Engine
{
	class Runtime : public App::Application
	{
		RYU_LOG_CATEGORY(Runtime);

	public:
		Runtime();
		inline constexpr World::World& GetWorld() noexcept { return m_world; }

	protected:
		virtual bool OnInit() override;
		virtual void OnShutdown() override;
		virtual void OnTick(f64 dt) override;

	private:
		World::World m_world;
	};
}
