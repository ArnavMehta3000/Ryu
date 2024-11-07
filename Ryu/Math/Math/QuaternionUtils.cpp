#include "QuaternionUtils.h"
#include <format>
namespace Ryu::Math
{
	std::string ToString(const SM::Quaternion& q)
	{
		return "Quat(" + std::to_string(q.x) + ", " + std::to_string(q.y) + ", " + std::to_string(q.z) + ", " + std::to_string(q.w) + ")"; 
	}
}

template<>
struct std::formatter<SM::Quaternion>
{
	auto parse(std::format_parse_context& ctx)
	{
		return ctx.begin();
	}

	auto format(const SM::Quaternion& q, std::format_context& ctx) const
	{
		return std::format_to(ctx.out(), "{}", Ryu::Math::ToString(q));
	}
};
