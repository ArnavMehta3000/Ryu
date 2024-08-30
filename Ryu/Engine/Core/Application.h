#pragma once
#include <Core/Includes.h>
#include <Core/Application/IApplication.h>

namespace Ryu
{
	class RYU_API Application : public IApplication
	{
	public:
		RYU_DISABLE_COPY_AND_MOVE(Application);

		Application(const Window::Config& wndConfig = Window::Config());
		virtual ~Application() = default;

		NODISCARD bool OnInit() override;
		void OnUpdate(MAYBE_UNUSED const f32 dt) override;
		void OnRender() override;
		void OnShutdown() override;
	};
}
