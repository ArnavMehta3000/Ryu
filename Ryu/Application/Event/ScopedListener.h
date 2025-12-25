#pragma once
#include "Application/Event/Event.h"
#include "Application/Event/EventDispatcher.h"
#include <optional>

namespace Ryu::Event
{
	template <IsEvent EventType>
	class ScopedListener
	{
		RYU_DISABLE_COPY(ScopedListener)
	public:
		ScopedListener()
			: m_dispatcher(nullptr)
			, m_handle(std::nullopt)
		{
		}

		template<typename CallbackType>
		ScopedListener(EventDispatcher& dispatcher, CallbackType&& callback)
			: m_dispatcher(&dispatcher)
			, m_handle(dispatcher.Subscribe<EventType>(std::forward<CallbackType>(callback)))
		{
		}

		ScopedListener(ScopedListener&& other) noexcept
			: m_dispatcher(other.m_dispatcher), m_handle(other.m_handle)
		{
			other.m_dispatcher = nullptr;
		}

		~ScopedListener()
		{
			Unsubscribe();
		}

		// Initialize after construction
		template<typename CallbackType>
		void Initialize(EventDispatcher& dispatcher, CallbackType&& callback)
		{
			Unsubscribe();
			m_dispatcher = &dispatcher;
			m_handle = dispatcher.Subscribe<EventType>(std::forward<CallbackType>(callback));
		}

		void Unsubscribe()
		{
			if (m_dispatcher && m_handle)
			{
				m_dispatcher->Unsubscribe(*m_handle);
				m_handle.reset();
			}
		}

		inline bool IsSubscribed() const
		{
			return m_dispatcher && m_handle.has_value();
		}

		ScopedListener& operator=(ScopedListener&& other) noexcept
		{
			if (this != &other)
			{
				Unsubscribe();
				
				m_dispatcher = other.m_dispatcher;
				m_handle = other.m_handle;
				other.m_dispatcher = nullptr;
			}
			return *this;
		}

	private:
		EventDispatcher*              m_dispatcher;
		std::optional<ListenerHandle> m_handle;
	};
}
