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
		StaticServiceRegistrar(auto&& factory, Utils::ServiceOptions options = {})
		{
			std::ignore = Globals::GetServiceLocator().RegisterService<T>(std::forward<decltype(factory)>(factory), options);
		}
	};
}

#define RYU_REGISTER_STATIC_SERVICE(Type, Factory, ...) \
    static inline ::Ryu::Utils::Internal::StaticServiceRegistrar<Type> g_##Type##Registrar{ Factory, __VA_ARGS__ }
