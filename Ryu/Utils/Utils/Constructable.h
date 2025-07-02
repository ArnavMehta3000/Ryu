#pragma once
#include "Common/Concepts.h"

namespace Ryu::Utils
{
	template <typename T, typename... Args>
	concept HasOnConstruct = requires(T * obj, Args&&... args)
	{
		obj->OnConstruct(std::forward<Args>(args)...);
	};

	template<typename T>
	concept HasParameterlessOnConstruct = requires(T * obj)
	{
		obj->OnConstruct();
	};
	
	template<typename T>
		requires HasParameterlessOnConstruct<T>
	void InvokeOnConstruct(T* obj)
	{
		if (obj) obj->OnConstruct();
	}

	template<typename T, typename... OnConstructArgs>
		requires Utils::HasOnConstruct<T, OnConstructArgs...>
	void InvokeOnConstruct(T* obj, OnConstructArgs&&... args)
	{
		if (obj) obj->OnConstruct(std::forward<OnConstructArgs>(args)...);
	}
}
