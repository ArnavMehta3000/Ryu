#pragma once
#include "Common/API.h"
#include "Common/StandardTypes.h"
#include "Utils/Timer.h"
#include "Memory/Ref.h"
#include "Window/Window.h"
#include <Elos/Application/AppBase.h>
#include <memory>

namespace Ryu::App
{
	/**
	 * @brief The Application class
	 */
	class Application : public Elos::AppBase, public Memory::RefCounted
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
		RYU_API void Tick(const Utils::TimeInfo& timeInfo);

		RYU_API inline NODISCARD Elos::WindowEventSignals& GetWindowEventSignals() { return m_windowEventSignals; }

	protected:
		/**
		 * @brief Called when the application is initialized
		 * @return true if successful
		 */
		virtual RYU_API bool OnInit();

		/**
		 * @brief Called when the application is shutdown
		 */
		virtual RYU_API void OnShutdown();

		/**
		 * @brief Called when the application is updates
		 * @param timeInfo frame time info
		 */
		virtual RYU_API void OnTick(const Utils::TimeInfo&) {}

		virtual RYU_API void GetWindowCreateInfo(Elos::WindowCreateInfo& outCreateInfo) override;

		virtual void RYU_API OnWindowClosedEvent();
		virtual void RYU_API OnWindowFocusLostEvent(const Elos::Event::FocusLost&) {}
		virtual void RYU_API OnWindowFocusGainedEvent(const Elos::Event::FocusGained&) {}
		virtual void RYU_API OnWindowMouseEnteredEvent(const Elos::Event::MouseEntered&) {}
		virtual void RYU_API OnWindowMouseLeftEvent(const Elos::Event::MouseLeft&) {}
		virtual void RYU_API OnWindowResizedEvent(const Elos::Event::Resized&) {}
		virtual void RYU_API OnWindowTextInputEvent(const Elos::Event::TextInput&) {}
		virtual void RYU_API OnWindowKeyPressedEvent(const Elos::Event::KeyPressed&) {};
		virtual void RYU_API OnWindowKeyReleasedEvent(const Elos::Event::KeyReleased&) {}
		virtual void RYU_API OnWindowMouseWheelScrolledEvent(const Elos::Event::MouseWheelScrolled&) {}
		virtual void RYU_API OnWindowMouseButtonPressedEvent(const Elos::Event::MouseButtonPressed&) {}
		virtual void RYU_API OnWindowMouseButtonReleasedEvent(const Elos::Event::MouseButtonReleased&) {}
		virtual void RYU_API OnWindowMouseMovedEvent(const Elos::Event::MouseMoved&) {}
		virtual void RYU_API OnWindowMouseMovedRawEvent(const Elos::Event::MouseMovedRaw&) {}

	private:
		void ConfigureConnections();

	protected:
		Elos::WindowEventConnections m_windowEventConnections;
	};

	namespace Internal
	{
		void SetUpDefaultLogger();
	}

	// The application interface class (contains core systems)
	class IApplication
	{
	public:
		virtual ~IApplication() = default;
		i32 Y = 2;
	};


	class App
	{
	public:
		App() = default;

		void Run();

		RYU_PROPERTY(Window, std::shared_ptr<Window::Window>);
		inline void SetWindow(std::shared_ptr<Window::Window> window) { m_window = window; }
		inline NODISCARD std::shared_ptr<Window::Window> GetWindow() const { return m_window; }

		RYU_PROPERTY(Application, std::shared_ptr<IApplication>);
		inline void SetApplication(std::shared_ptr<IApplication> app) { m_appInterface = app; }
		
		template <class T = IApplication> requires std::is_base_of_v<IApplication, T>
		inline NODISCARD auto GetApplication() const
		{
			if constexpr (std::is_same_v<T, IApplication>)
			{
				return m_appInterface;
			}
			else 
			{
				return std::dynamic_pointer_cast<T>(m_appInterface);
			}
		}

	private:
		std::shared_ptr<Window::Window> m_window;
		std::shared_ptr<IApplication> m_appInterface;
	};
}
