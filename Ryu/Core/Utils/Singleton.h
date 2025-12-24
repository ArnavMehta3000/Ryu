#pragma once
#include "Core/Common/ObjectMacros.h"

namespace Ryu::Utils
{
	/*
	* Use this class by deriving from it and declaring the child as a friend of this class
	* Use the macro RYU_SINGLETON_DECLARE(ChildClassName) to declare the friend
	*/

	template <typename T>
	class Singleton
	{
	public:
		[[nodiscard]] static T& Get()
		{
			static T instance;
			return instance;
		}

	protected:
		Singleton() = default;
		~Singleton() = default;

		Singleton(const Singleton&) = delete;
		Singleton& operator=(const Singleton&) = delete;
		Singleton(Singleton&&) = delete;
		Singleton& operator=(Singleton&&) = delete;
	};
}

#define RYU_SINGLETON_DECLARE(ChildClassName) friend class ::Ryu::Utils::Singleton<ChildClassName>
