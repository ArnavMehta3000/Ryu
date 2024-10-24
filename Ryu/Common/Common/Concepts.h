#pragma once
#include <type_traits>
#include <concepts>

namespace Ryu
{
	template <typename T>
	concept IsPt = std::is_pointer_v<T>;

	template <typename T>
	concept IsFunction = std::is_function_v<std::remove_pointer_t<T>>;

	template <typename T>
	concept IsFuncPtr = std::is_pointer_v<T> && std::is_function_v<std::remove_pointer_t<T>>;

	template <typename T>
	concept IsEnum = std::is_enum_v<T>;

	template <typename Child, typename Parent>
	concept IsDerived = std::is_base_of_v<Child, Parent>;

	template <typename T, typename U>
	concept IsSame = std::is_same_v<T, U>;

	template <typename T>
	concept IsTickable = requires(T* t)
	{
		{ t->OnTick(double()) } -> std::same_as<void>;
	};
}
