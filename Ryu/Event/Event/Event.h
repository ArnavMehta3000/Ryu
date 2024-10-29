#pragma once
#include "Utils/Random/Random.h"
#include <functional>
#include <vector>

namespace Ryu::Events
{
	template <typename... Args>
	class Event
	{
	public:
		using CallbackType = std::function<void(Args...)>;

	private:
		struct Delegate
		{
			u64 ID{ 0 };
			CallbackType Callback;
		};

	public:
		using DelegateHandle = decltype(std::declval<Delegate>().ID);
		
		template<typename T>
		NODISCARD DelegateHandle Add(T* objectPtr,  void (T::*func)(Args...))
		{
			Delegate d;
			
			d.ID = Utils::Random::U64();
			d.Callback = [objectPtr, func](Args... args)
			{
				(objectPtr->*func)(std::forward<Args>(args)...);
			};

			DelegateHandle handle = d.ID;
			m_callbacks.push_back(std::move(d));

			return handle;
		}

		NODISCARD DelegateHandle Add(CallbackType callback)
		{
			Delegate d;
			d.ID = Utils::Random::U64();
			d.Callback = std::move(callback);

			DelegateHandle handle = d.ID;
			m_callbacks.push_back(std::move(d));

			return handle;
		}

		template <typename Callable>
		NODISCARD DelegateHandle Add(Callable&& callable)
		{
			Delegate d;
			d.ID = Utils::Random::U64();
			d.Callback = std::forward<Callable>(callable);

			DelegateHandle handle = d.ID;
			m_callbacks.push_back(std::move(d));

			return handle;
		}

		void Remove(DelegateHandle handle)
		{
			m_callbacks.erase(
				std::remove_if(m_callbacks.begin(), m_callbacks.end(),
					[&handle](const Delegate& delegate)
					{
						return handle == delegate.ID;
					}),
				m_callbacks.end());
		}


		void Broadcast(Args... args) const
		{
			if (m_callbacks.empty())
			{
				return;
			}
			
			for (auto& d : m_callbacks)
			{
				d.Callback(std::forward<Args>(args)...);
			}
		}

	private:
		std::vector<Delegate> m_callbacks;
	};
}

// Used to declare a event and its delegate handle
#define RYU_DECLARE_EVENT(Name, ...)\
::Ryu::Events::Event<__VA_ARGS__> Name;\
using RYU_CONCAT(Name, DelegateHandle) = ::Ryu::Events::Event<__VA_ARGS__>::DelegateHandle 
