#pragma once
#include "App/Events/WindowEvents.h"

namespace Ryu::App
{
	// Abstract interface for window event listeners
	class IWindowEventListener
		: public Ryu::Events::IEventListener<Events::OnWindowClose>
		, public Ryu::Events::IEventListener<Events::OnWindowStateChange>
		, public Ryu::Events::IEventListener<Events::OnWindowResize>
		, public Ryu::Events::IEventListener<Events::OnWindowFocusChange>
	{
	public:
		virtual void OnEvent(MAYBE_UNUSED const Events::OnWindowClose& event)       = 0;
		virtual void OnEvent(MAYBE_UNUSED const Events::OnWindowStateChange& event) = 0;
		virtual void OnEvent(MAYBE_UNUSED const Events::OnWindowResize& event)      = 0;
		virtual void OnEvent(MAYBE_UNUSED const Events::OnWindowFocusChange& event) = 0;
	};
}
