#pragma once
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