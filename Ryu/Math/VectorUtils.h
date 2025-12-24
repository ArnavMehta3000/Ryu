#pragma once
#include "Core/Common/API.h"
#include "Math/Math.h"
#include <string>

namespace Ryu::Math
{
	RYU_API std::string ToString(const SM::Vector2& v);
	RYU_API std::string ToString(const SM::Vector3& v);
	RYU_API std::string ToString(const SM::Vector4& v);
}
