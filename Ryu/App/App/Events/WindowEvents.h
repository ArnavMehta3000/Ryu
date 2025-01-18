#pragma once
#include "Common/ObjectMacros.h"
#include "Event/IEventListener.h"

namespace Ryu::App
{
	class Window;
	
	/**
	 * @brief Represents the state of the window
	 */
	enum class WindowState
	{
		/**
		 * @brief When window has been resized, but minimized or maximized doesn't apply
		 */
		Restored  = 0,

		/**
		 * @brief When window is maximized
		 */
		Maximized = 1,

		/**
		 * @brief When window is minimized
		 */
		Minimized = 2
	};

	/**
	 * @brief  Represents the focus state of the window
	 */
	enum class WindowFocus
	{
		/**
		 * @brief When the window just gained focus
		 */
		Gained,

		/**
		 * @brief When the window just lost focus
		 */
		Lost
	};

	namespace Events
	{
		namespace Internal
		{
			struct RYU_API WindowEvent
			{
				Window* Window{ nullptr };
			};
		}

		/**
		 * @brief Event dispatched when the window is closed
		 */
		struct RYU_API OnWindowClose : public Internal::WindowEvent {};

		/**
		 * @brief Event dispatched when the window state is changed
		 */
		struct RYU_API OnWindowStateChange : public Internal::WindowEvent
		{
			/**
			 * @brief The new window state
			 */
			WindowState State{ WindowState::Restored };
			
			/**
			 * @brief The new window client width
			 */
			u32 Width{ 0 };
			
			/**
			 * @brief The new window client height
			 */
			u32 Height{ 0 };
		};

		/**
		 * @brief Event dispatched when the window is resized
		 */
		struct RYU_API OnWindowResize : public Internal::WindowEvent
		{
			/**
			 * @brief The new window client width
			 */
			u32 Width{ 0 };

			/**
			 * @brief The new window client height
			 */
			u32 Height{ 0 };
		};

		/**
		 * @brief Event dispatched when the window focus state is changed
		 */
		struct RYU_API OnWindowFocusChange : public Internal::WindowEvent
		{
			/**
			 * @brief The new focus state
			 */
			WindowFocus Focus{ WindowFocus::Lost };
		};
	}
}
