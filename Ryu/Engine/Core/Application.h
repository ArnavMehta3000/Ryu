#pragma once
#include <Core/Includes.h>
#include <Core/Application/IApplication.h>
#include <Engine/Input/InputEvents.h>

namespace Ryu
{
	class RYU_API Application 
		: public IApplication
		, public InputEventListener
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

		virtual void OnEvent(const Events::OnKeyDown& event) override;
		virtual void OnEvent(const Events::OnKeyUp& event) override;
		virtual void OnEvent(const Events::OnMouseButtonUp& event) override;
		virtual void OnEvent(const Events::OnMouseButtonDown& event) override;
		virtual void OnEvent(const Events::OnMouseDblClick& event) override;
		virtual void OnEvent(const Events::OnMouseMove& event) override;
		virtual void OnEvent(const Events::OnMouseMoveRaw& event) override;
		virtual void OnEvent(const Events::OnMouseWheel& event) override;

	private:
		static Application* s_instance;
	};
}
