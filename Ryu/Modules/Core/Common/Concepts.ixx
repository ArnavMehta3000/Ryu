module;
#include <type_traits>
#include <concepts>

export module Ryu.Core.Common:Concepts;

export namespace Ryu
{
	/**
	 * @brief Concept to check if a type is a pointer
	 * @tparam T The type to check
	 */
	template <typename T>
	concept IsPtr = std::is_pointer_v<T>;

	/**
	 * @brief Concept to check if a type is a function
	 * @tparam T The type to check
	 */
	template <typename T>
	concept IsFunction = std::is_function_v<std::remove_pointer_t<T>>;

	/**
	 * @brief Concept to check if a type is a function pointer
	 * @tparam T The type to check
	 */
	template <typename T>
	concept IsFuncPtr = std::is_pointer_v<T> && std::is_function_v<std::remove_pointer_t<T>>;

	/**
	 * @brief Concept to check if a type is an enum
	 * @tparam T The type to check
	 */
	template <typename T>
	concept IsEnum = std::is_enum_v<T>;

	/**
	 * @brief Concept to check if a class is derived from another
	 * @tparam Child The class to check
	 * @tparam Parent The base class
	 */
	template <typename Parent, typename Child>
	concept IsDerived = std::is_base_of_v<Parent, Child>;

	/**
	 * @brief Concept to check if two types are the same
	 * @tparam T The first type
	 * @tparam U The second type
	 */
	template <typename T, typename U>
	concept IsSame = std::is_same_v<T, U>;

	/**
	 * @brief Concept to check if a type is tickable (has a `Tick(f64)` method)
	 * @tparam T The type to check
	 */
	template <typename T>
	concept IsTickable = requires(T* t)
	{
		{ t->Tick(double()) } -> std::same_as<void>;
	};

	
	/**
	 * @brief Concept to check if a type is complete (has a valid `sizeof()` method)
	 * @tparam T The type to check
	 */
	template <typename T>
	concept IsComplete = requires { sizeof(T); };
}
