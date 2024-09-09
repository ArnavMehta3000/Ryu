#pragma once
#include <Core/Event/EventListener.h>
#include <functional>

namespace Ryu::Input
{
	namespace Events
	{
		struct RYU_API OnKeyDown
		{

		};

		struct RYU_API OnKeyUp
		{

		};
	}

	using OnKeyDownCallback = std::function<void(const Events::OnKeyDown&)>;
	using OnKeyUpCallback = std::function<void(const Events::OnKeyUp&)>;


	struct RYU_API InputCallbacks
	{
		OnKeyDownCallback OnKeyDown;
		OnKeyUpCallback OnKeyUp;

		InputCallbacks() = default;
	};

	class RYU_API InputListener 
		: public EventListener<Input::Events::OnKeyDown>
		, public EventListener<Input::Events::OnKeyUp>
	{
		virtual void OnEvent(MAYBE_UNUSED const Input::Events::OnKeyDown& event) {}
		virtual void OnEvent(MAYBE_UNUSED const Input::Events::OnKeyUp& event) {}
	};
}