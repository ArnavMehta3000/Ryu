#pragma once
#include "Common/API.h"
#include "Common/StandardTypes.h"
#include <Elos/Application/AppBase.h>
#include <memory>

namespace Ryu::App
{
	/**
	 * @brief The Application class
	 */
	class Application : private Elos::AppBase
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
		inline RYU_API NODISCARD Elos::Window* GetWindow() const { return m_window.get(); }

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

		virtual RYU_API void GetWindowCreateInfo(Elos::WindowCreateInfo& outCreateInfo) override;
	private:


	private:
		bool m_isRunning;
	};
}
