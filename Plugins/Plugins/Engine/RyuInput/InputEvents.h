#pragma once
#include <Core/ObjectMacros.h>
#include <Plugins/Engine/RyuInput/KeyCode.h>
#include <Core/Event/EventListener.h>
#include <functional>

namespace Ryu::Input
{
	namespace Events
	{
		namespace Internal
		{
			struct RYU_API OnKeyEvent
			{
				KeyCode Key;
			};
		}

		struct RYU_API OnKeyDown : public Internal::OnKeyEvent 
		{
			explicit OnKeyDown(KeyCode key)
			{
				Key = key;
			}
		};

		struct RYU_API OnKeyUp : public Internal::OnKeyEvent
		{
			explicit OnKeyUp(KeyCode key)
			{
				Key = key;
			}
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