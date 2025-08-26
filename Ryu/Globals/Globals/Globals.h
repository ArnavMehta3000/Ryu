#pragma once
#include "Utils/ServiceLocator.h"

namespace Ryu::Globals
{
	extern const bool g_isDebug;

	extern bool IsDebuggerAttached();

	extern Utils::ServiceLocator& GetServiceLocator();
}

namespace Ryu::Utils::Internal
{
	template <typename T>
	struct StaticServiceRegistrar
	{
		StaticServiceRegistrar(auto&& factory)
		{
			std::ignore = Globals::GetServiceLocator().RegisterService<T>(std::forward<decltype(factory)>(factory));
		}
	};
}

#define RYU_REGISTER_STATIC_SERVICE(Type, Factory) \
    inline static ::Ryu::Utils::Internal::StaticServiceRegistrar<Type> g_##Type##Registrar{ Factory }
