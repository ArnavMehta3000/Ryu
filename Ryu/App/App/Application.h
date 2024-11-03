#pragma once
#include "App/Window.h"
#include "Input/InputSystem.h"
#include "Event/Event.h"
#include <memory>

namespace Ryu::App
{
	/**
	 * @brief The Application class
	 */
	class Application
		: public IWindowEventListener
		, public Input::IInputEventListener
		, public ITickable<Application>
	{
	public:
		/**
		 * @brief Construct a new Application object
		 */
		RYU_API Application();

		/**
		 * @brief Virtual destructor
		 */
		virtual RYU_API ~Application();

		/**
		 * @brief Initialize the application
		 * @details This will call OnInit()
		 * @return true if successful
		 */
		RYU_API bool Init();

		/**
		 * @brief Shutdown the application
		 */
		RYU_API void Shutdown();

		/**
		 * @brief Update the application
		 * @param dt delta time
		 */
		RYU_API void Tick(f64 dt);

		/**
		 * @brief Stop the application
		 * @details This will call OnShutdown()
		 */
		RYU_API void StopRunning();

		/**
		 * @brief Get the window of the application
		 * @return Shared pointer to the window
		 */
		inline RYU_API NODISCARD std::shared_ptr<Window> GetWindow() const { return m_window; }

		/**
		 * @brief Get if the application is running
		 * @return true if the application is running
		 */
		inline RYU_API NODISCARD bool IsRunning() const { return m_isRunning; }

	protected:
		/**
		 * @brief Called when the application is initialized
		 * @return true if successful
		 */
		virtual RYU_API bool OnInit() = 0;

		/**
		 * @brief Called when the application is shutdown
		 */
		virtual RYU_API void OnShutdown() = 0;

		/**
		 * @brief Called when the application is updates
		 * @param dt delta time
		 */
		virtual RYU_API void OnTick(f64 dt) = 0;

	private:
		// Inherited via IWindowEventListener
		void OnEvent(const App::Events::OnWindowClose& event) override;
		void OnEvent(const App::Events::OnWindowStateChange& event) override;
		void OnEvent(const App::Events::OnWindowResize& event) override;
		void OnEvent(const App::Events::OnWindowFocusChange& event) override;

		// Inherited via IInputEventListener
		void OnEvent(const Input::Events::OnKeyDown& event) override;
		void OnEvent(const Input::Events::OnKeyUp& event) override;
		void OnEvent(const Input::Events::OnMouseButtonUp& event) override;
		void OnEvent(const Input::Events::OnMouseButtonDown& event) override;
		void OnEvent(const Input::Events::OnMouseDblClick& event) override;
		void OnEvent(const Input::Events::OnMouseMove& event) override;
		void OnEvent(const Input::Events::OnMouseMoveRaw& event) override;
		void OnEvent(const Input::Events::OnMouseWheel& event) override;

	public:
		RYU_DECLARE_EVENT(OnWindowClose,       const App::Events::OnWindowClose&);
		RYU_DECLARE_EVENT(OnWindowStateChange, const App::Events::OnWindowStateChange&);
		RYU_DECLARE_EVENT(OnWindowResize,      const App::Events::OnWindowResize&);
		RYU_DECLARE_EVENT(OnWindowFocusChange, const App::Events::OnWindowFocusChange&);
		RYU_DECLARE_EVENT(OnKeyDown,           const Input::Events::OnKeyDown&);
		RYU_DECLARE_EVENT(OnKeyUp,             const Input::Events::OnKeyUp&);
		RYU_DECLARE_EVENT(OnMouseButtonUp,     const Input::Events::OnMouseButtonUp&);
		RYU_DECLARE_EVENT(OnMouseButtonDown,   const Input::Events::OnMouseButtonDown&);
		RYU_DECLARE_EVENT(OnMouseDblClick,     const Input::Events::OnMouseDblClick&);
		RYU_DECLARE_EVENT(OnMouseMove,         const Input::Events::OnMouseMove&);
		RYU_DECLARE_EVENT(OnMouseMoveRaw,      const Input::Events::OnMouseMoveRaw&);
		RYU_DECLARE_EVENT(OnMouseWheel,        const Input::Events::OnMouseWheel&);

	private:
		std::unique_ptr<Input::InputSystem> m_inputSystem;
		std::shared_ptr<Window>             m_window;
		bool                                m_isRunning;
		bool                                m_hasFocus;
	};
}
