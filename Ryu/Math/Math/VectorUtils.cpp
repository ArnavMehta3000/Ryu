#include "VectorUtils.h"
#include <format>

namespace Ryu::Math
{
	std::string ToString(const SM::Vector2& v)
	{
		return "Vec2(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")";
	}

	std::string ToString(const SM::Vector3& v)
	{
		return "Vec3(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ")";
	}

	std::string ToString(const SM::Vector4& v)
	{
		return "Vec4(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ", " + std::to_string(v.w) + ")";
	}
}

template <>
struct std::formatter<SM::Vector2>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return ctx.begin();
	}

	auto format(const SM::Vector2& vec, std::format_context& ctx) const
	{
		return std::format_to(ctx.out(), "{}", Ryu::Math::ToString(vec));
	}

};

template <>
struct std::formatter<SM::Vector3>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return ctx.begin();
	}

	auto format(const SM::Vector3& vec, std::format_context& ctx) const
	{
		return std::format_to(ctx.out(), "{}", Ryu::Math::ToString(vec));
	}
};

template <>
struct std::formatter<SM::Vector4>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return ctx.begin();
	}

	auto format(const SM::Vector4& vec, std::format_context& ctx) const
	{
		return std::format_to(ctx.out(), "{}", Ryu::Math::ToString(vec));
	}
};
