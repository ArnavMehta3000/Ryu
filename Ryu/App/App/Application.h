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
		~Application();

		bool Init();
		void Shutdown();

		void OnTick(MAYBE_UNUSED f64 dt);

		inline NODISCARD std::shared_ptr<Window> GetWindow() const { return m_window; }
		inline NODISCARD bool IsRunning() const { return m_isRunning; }


	private:
		// Inherited via IWindowEventListener
		void OnEvent(MAYBE_UNUSED const App::Events::OnWindowClose& event) override;
		void OnEvent(MAYBE_UNUSED const App::Events::OnWindowStateChange& event) override;
		void OnEvent(MAYBE_UNUSED const App::Events::OnWindowResize& event) override;
		void OnEvent(MAYBE_UNUSED const App::Events::OnWindowFocusChange& event) override;

		// Inherited via IInputEventListener
		void OnEvent(MAYBE_UNUSED const Input::Events::OnKeyDown& event) override;
		void OnEvent(MAYBE_UNUSED const Input::Events::OnKeyUp& event) override;
		void OnEvent(MAYBE_UNUSED const Input::Events::OnMouseButtonUp& event) override;
		void OnEvent(MAYBE_UNUSED const Input::Events::OnMouseButtonDown& event) override;
		void OnEvent(MAYBE_UNUSED const Input::Events::OnMouseDblClick& event) override;
		void OnEvent(MAYBE_UNUSED const Input::Events::OnMouseMove& event) override;
		void OnEvent(MAYBE_UNUSED const Input::Events::OnMouseMoveRaw& event) override;
		void OnEvent(MAYBE_UNUSED const Input::Events::OnMouseWheel& event) override;

	private:
		std::unique_ptr<Input::InputSystem> m_inputSystem;
		std::shared_ptr<Window>             m_window;
		bool                                m_isRunning;
		bool                                m_hasFocus;
	};
}
