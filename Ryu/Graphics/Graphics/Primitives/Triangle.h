#pragma once
#include "Graphics/Primitives/VertexTypes.h"
#include <array>

namespace Ryu::Gfx::Primitives
{
	static constexpr std::array<VertexPosCol, 3> TriangleVerticesPosCol =
	{
		VertexPosCol
		{ {  0.0f,   0.25f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
		{ {  0.25f, -0.25f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
		{ { -0.25f, -0.25f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
	};

	static constexpr auto TriangleVertexBufferSize = sizeof(TriangleVerticesPosCol);
}
