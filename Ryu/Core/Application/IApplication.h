#pragma once
#include <CoreDLLDefines.h>
#include <Core/ObjectMacros.h>
#include <Core/Application/Window.h>
#include <memory>

namespace Ryu
{
	class RYU_API IApplication
	{
		friend class Engine;
	public:
		RYU_DISABLE_COPY_AND_MOVE(IApplication);

		IApplication(const Window::Config& wndConfig = Window::Config()) : m_window(wndConfig) {}
		virtual ~IApplication() = default;

		NODISCARD const Window& GetWindow() const { return m_window; }


		virtual NODISCARD bool OnInit() = 0;
		virtual void OnUpdate(const f32 dt) = 0;
		virtual void OnRender() = 0;
		virtual void OnShutdown() = 0;

	protected:
		Window m_window;
	};
}