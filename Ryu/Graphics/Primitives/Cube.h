#pragma once
#include "Common/StandardTypes.h"
#include "Graphics/Primitives/VertexTypes.h"
#include <array>

namespace Ryu::Gfx::Primitives
{
	static constexpr std::array<VertexPosCol, 8> CubeVerticesPosCol =
	{
		VertexPosCol
        { { -0.25f, -0.25f, -0.25f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
        { {  0.25f, -0.25f, -0.25f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
        { {  0.25f,  0.25f, -0.25f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
        { { -0.25f,  0.25f, -0.25f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        { { -0.25f, -0.25f,  0.25f }, { 1.0f, 0.0f, 1.0f, 1.0f } },
        { {  0.25f, -0.25f,  0.25f }, { 1.0f, 1.0f, 1.0f, 1.0f } },
        { {  0.25f,  0.25f,  0.25f }, { 0.0f, 0.0f, 0.0f, 1.0f } },
        { { -0.25f,  0.25f,  0.25f }, { 1.0f, 1.0f, 0.0f, 1.0f } },
	};

	static constexpr auto CubeVertexBufferSize = sizeof(CubeVerticesPosCol);

    const std::array<u16, 36> CubeIndices
    {
        0, 1, 3, 3, 1, 2,
        1, 5, 2, 2, 5, 6,
        5, 4, 6, 6, 4, 7,
        4, 0, 7, 7, 0, 3,
        3, 2, 7, 7, 2, 6,
        4, 5, 0, 0, 5, 1
    };

	static const auto CubeIndexBufferSize = sizeof(CubeIndices);
}
