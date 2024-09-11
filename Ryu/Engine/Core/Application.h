#pragma once
#include <Core/Includes.h>
#include <Core/Application/IApplication.h>
#include <Plugins/Engine/RyuInput/InputEvents.h>

namespace Ryu
{
	class RYU_API Application 
		: public IApplication
		, Input::InputEventListener
	{
	public:
		RYU_DISABLE_COPY_AND_MOVE(Application);

		Application(const Window::Config& wndConfig = Window::Config());
		virtual ~Application() = default;

		NODISCARD bool OnInit() override;
		void OnUpdate(MAYBE_UNUSED const f32 dt) override;
		void OnRender() override;
		void OnShutdown() override;

		virtual void OnEvent(const Input::Events::OnKeyDown& event) override;
		virtual void OnEvent(const Input::Events::OnKeyUp& event) override;
		virtual void OnEvent(const Input::Events::OnMouseButtonUp& event) override;
		virtual void OnEvent(const Input::Events::OnMouseButtonDown& event) override;
		virtual void OnEvent(const Input::Events::OnMouseDblClick& event) override;
		virtual void OnEvent(const Input::Events::OnMouseMove& event) override;
		virtual void OnEvent(const Input::Events::OnMouseMoveRaw& event) override;
		virtual void OnEvent(const Input::Events::OnMouseWheel& event) override;
	};
}
