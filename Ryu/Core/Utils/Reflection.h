#pragma once
#include "Core/Common/Concepts.h"
#include <tuple>
#include <string_view>

namespace Ryu::Reflection
{
#undef GetClassName  // From windows

	template<typename T>
	std::string_view RegisterClassName();

	template<typename T>
	auto RegisterClassAttributes();

	// Concept to check if a type is reflected
	template<typename T>
	concept IsReflected = requires
	{
		{ RegisterClassName<T>() } -> std::convertible_to<std::string_view>;
		{ RegisterClassAttributes<T>() };
	};

	template <typename Type, typename Attribs>
	struct ReflectionClass
	{
		inline static std::string_view Name = RegisterClassName<Type>();
		inline static Attribs Attributes = RegisterClassAttributes<Type>();
	};

	template <typename... Args>
	auto MakeAttributes(Args&&... args) { return std::make_tuple(std::forward<Args>(args)...); }

	template<IsReflected T>
	constexpr std::string_view GetName() { return ReflectionClass<T, decltype(RegisterClassAttributes<T>())>::Name; }

	template<IsReflected T>
	constexpr auto& GetAttributes() { return ReflectionClass<T, decltype(RegisterClassAttributes<T>())>::Attributes; }

	template<IsReflected T, std::size_t Index>
	constexpr auto& GetAttribute() { return std::get<Index>(GetReflectedClassAttributes<T>()); }

	template<IsReflected T>
	constexpr std::size_t GetAttributesCount() { return std::tuple_size_v<decltype(RegisterClassAttributes<T>())>; }

	template<IsReflected T, typename Func>
		requires std::invocable<Func, decltype(std::get<0>(RegisterClassAttributes<T>()))>
	void ForEachAttribute(Func&& func)
	{
		std::apply([&](auto&&... attrs)
		{
				(..., func(attrs));
		}, GetAttributes<T>());
	}

	template <typename Class, typename Var>
	class Attribute
	{
	public:
		using Ptr = Var Class::*;
		using Type = Var;

		Attribute(std::string_view name, Ptr ptr) : m_name(name), m_ptr(ptr) {}

		[[nodiscard]] std::string_view GetName() const { return m_name; }
		[[nodiscard]] const Var& GetValue(const Class& obj) const { return obj.*m_ptr; }
		[[nodiscard]] Var& GetValue(Class& obj) const { return obj.*m_ptr; }

		template<typename T>
		void SetValue(Class& obj, T&& value) const { obj.*m_ptr = std::forward<T>(value); }

	private:
		std::string_view m_name;
		Ptr              m_ptr = nullptr;
	};
}

#define RYU_ENABLE_REFLECTION(Class)                                                   \
    template<typename> friend std::string_view Ryu::Reflection::RegisterClassName(); \
    template<typename> friend auto Ryu::Reflection::RegisterClassAttributes();

#define RYU_CLASS_ATTRIB(Class, Var) ::Ryu::Reflection::Attribute(#Var, &Class::Var)

#define RYU_REFLECTED_CLASS(Class, FriendlyName, ...)                                    \
namespace Ryu::Reflection                                                                \
{                                                                                        \
	template<>                                                                           \
	inline std::string_view RegisterClassName<Class>() { return FriendlyName; }          \
                                                                                         \
	template <>                                                                          \
	inline auto RegisterClassAttributes<Class>() { return MakeAttributes(__VA_ARGS__); } \
}
