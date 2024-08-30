#pragma once
#include <Core/Includes.h>
#include <Core/Application/IApplication.h>

namespace Ryu
{
	class RYU_API Application : IApplication
	{
	public:
		RYU_DISABLE_COPY_AND_MOVE(Application);

		Application(const Window::Config& wndConfig = Window::Config());
		virtual ~Application() = default;

		NODISCARD bool OnInit() override;
		void OnUpdate() override;
		void OnRender() override;
		void OnShutdown() override;

		void Run();
	};
}
