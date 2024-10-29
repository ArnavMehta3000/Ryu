#pragma once
#include "App/Window.h"
#include "Input/InputSystem.h"
#include <memory>

namespace Ryu::App
{
	class Application
		: public IWindowEventListener
		, public Input::IInputEventListener
		, public ITickable<Application>
	{
	public:
		Application();
		virtual ~Application();

		bool Init();
		void Shutdown();
		void Tick(f64 dt);

		void StopRunning();

		inline NODISCARD std::shared_ptr<Window> GetWindow() const { return m_window; }
		inline NODISCARD bool IsRunning() const { return m_isRunning; }

	protected:
		virtual bool OnInit() = 0;
		virtual void OnShutdown() = 0;
		virtual void OnTick(f64 dt) = 0;

	private:
		// Inherited via IWindowEventListener
		virtual void OnEvent(MAYBE_UNUSED const App::Events::OnWindowClose& event) override;
		virtual void OnEvent(MAYBE_UNUSED const App::Events::OnWindowStateChange& event) override;
		virtual void OnEvent(MAYBE_UNUSED const App::Events::OnWindowResize& event) override;
		virtual void OnEvent(MAYBE_UNUSED const App::Events::OnWindowFocusChange& event) override;

		// Inherited via IInputEventListener
		virtual void OnEvent(MAYBE_UNUSED const Input::Events::OnKeyDown& event) override;
		virtual void OnEvent(MAYBE_UNUSED const Input::Events::OnKeyUp& event) override;
		virtual void OnEvent(MAYBE_UNUSED const Input::Events::OnMouseButtonUp& event) override;
		virtual void OnEvent(MAYBE_UNUSED const Input::Events::OnMouseButtonDown& event) override;
		virtual void OnEvent(MAYBE_UNUSED const Input::Events::OnMouseDblClick& event) override;
		virtual void OnEvent(MAYBE_UNUSED const Input::Events::OnMouseMove& event) override;
		virtual void OnEvent(MAYBE_UNUSED const Input::Events::OnMouseMoveRaw& event) override;
		virtual void OnEvent(MAYBE_UNUSED const Input::Events::OnMouseWheel& event) override;

	private:
		std::unique_ptr<Input::InputSystem> m_inputSystem;
		std::shared_ptr<Window>             m_window;
		bool                                m_isRunning;
		bool                                m_hasFocus;
	};
}
