#pragma once
#include "Common/ObjectMacros.h"

namespace Ryu::Utils
{
	template <typename T>
	class Singleton
	{
	public:
		NODISCARD static T& Get()
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
#define RYU_SINGLETON_DECLARE(ClassName) friend class ::Ryu::Utils::Singleton<ClassName>
