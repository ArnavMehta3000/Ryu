#pragma once
#include "Utils/Random/Random.h"
#include <functional>
#include <vector>

namespace Ryu::Events
{
	/**
	 * @brief A lightweight wrapper around std::function
	 * @details Functions can subscribe to a callback and be called when the function is called
	 * @tparam ...Args The arguments passed to the callback
	 */
	template <typename... Args>
	class Event
	{
	public:
		/**
		 * @brief The type of the callback
		 */
		using CallbackType = std::function<void(Args...)>;

	private:
		/**
		 * @brief A struct that holds the callback and its ID
		 */
		struct Delegate
		{
			u64 ID{ 0 };
			CallbackType Callback;
		};

	public:
		/**
		 * @brief The type of the delegate handle
		 * @note This is always `u64` but can be used to alias event specific handles
		 */
		using DelegateHandle = decltype(std::declval<Delegate>().ID);
		
		/**
		 * @brief Add a member function as callback
		 * @tparam T The type of the object
		 * @param objectPtr A pointer to the object
		 * @param func The member function
		 * @return The delegate handle (nondiscardable)
		 */
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

		/**
		 * @brief Add `CallbackType` as callback
		 * @param callback The callback
		 * @return The delegate handle (nondiscardable)
		 */
		NODISCARD DelegateHandle Add(CallbackType callback)
		{
			Delegate d;
			d.ID = Utils::Random::U64();
			d.Callback = std::move(callback);

			DelegateHandle handle = d.ID;
			m_callbacks.push_back(std::move(d));

			return handle;
		}

		/**
		 * @brief Add a lambda as callback
		 * @param callable The lambda
		 * @return The delegate handle (nondiscardable)
		 */
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

		/**
		 * @brief Remove a callback from the event
		 * @param handle The delegate handle
		 */
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

		/**
		 * @brief Call all the callbacks
		 * @param ...args Arguments to pass to the callbacks
		 */
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
