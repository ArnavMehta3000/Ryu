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

		static Application* Get();

		Application(const Window::Config& wndConfig = Window::Config());
		virtual ~Application() = default;

		virtual NODISCARD bool OnInit() override;
		virtual void OnUpdate(const f32 dt) override;
		virtual void OnRender(const f32 dt) override;
		virtual void OnShutdown() override;

		virtual void OnEvent(const Input::Events::OnKeyDown& event) override;
		virtual void OnEvent(const Input::Events::OnKeyUp& event) override;
		virtual void OnEvent(const Input::Events::OnMouseButtonUp& event) override;
		virtual void OnEvent(const Input::Events::OnMouseButtonDown& event) override;
		virtual void OnEvent(const Input::Events::OnMouseDblClick& event) override;
		virtual void OnEvent(const Input::Events::OnMouseMove& event) override;
		virtual void OnEvent(const Input::Events::OnMouseMoveRaw& event) override;
		virtual void OnEvent(const Input::Events::OnMouseWheel& event) override;

	private:
		static Application* s_instance;
	};
}
