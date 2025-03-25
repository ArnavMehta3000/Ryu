#pragma once
#include "App/Application.h"
#include "Logger/LogCategory.h"

namespace Ryu::Engine
{
	class Runtime : public App::Application
	{
		RYU_LOG_CATEGORY(Runtime);

	public:
		Runtime();

	protected:
		virtual bool OnInit() override;
		virtual void OnShutdown() override;
		virtual void OnTick(f64 dt) override;
	};
}
