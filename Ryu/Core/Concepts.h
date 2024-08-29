#pragma once
#include <type_traits>

namespace Ryu
{
	template <typename T>
	concept IsPtr= std::is_pointer_v<T>;

	template <typename T>
	concept IsFunction = std::is_function_v<std::remove_pointer_t<T>>;

	template <typename T>
	concept IsFuncPtr= std::is_pointer_v<T> && std::is_function_v<std::remove_pointer_t<T>>;

	template <typename T, typename U>
	concept IsDerived = std::is_base_of_v<T, U>;

	template <typename T, typename U>
	concept IsSame = std::is_same_v<T, U>;
}