#pragma once
#include "Math/Math.h"
#include <array>

namespace Ryu::Gfx::Primitives
{
	struct TriangleVertexPosCol
	{
		Math::Vector3 Position;
		Math::Vector4 Color;
	};

	static constexpr std::array<TriangleVertexPosCol, 3> TriangleVerticesPosCol =
	{
		TriangleVertexPosCol
		{ { 0.0f, 0.25f, 0.0f    }, { 1.0f, 0.0f, 0.0f, 1.0f } },
		{ { 0.25f, -0.25f, 0.0f  }, { 0.0f, 1.0f, 0.0f, 1.0f } },
		{ { -0.25f, -0.25f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
	};

	static constexpr auto TriangleVertexBufferSize = sizeof(TriangleVerticesPosCol);
}
