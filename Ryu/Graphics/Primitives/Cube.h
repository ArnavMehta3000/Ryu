#pragma once
#include "Common/StandardTypes.h"
#include "Graphics/Primitives/VertexTypes.h"
#include <array>

namespace Ryu::Gfx::Primitives
{
	static constexpr std::array<VertexPosCol, 8> CubeVerticesPosCol =
	{
		VertexPosCol
        { { 0.25f, 0.25f, -0.25f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
        { {  -0.25f, 0.25f, -0.25f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
        { {  -0.25f,  0.25f, 0.25f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
        { { 0.25f,  0.25f, 0.25f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        { { 0.25f, -0.25f,  -0.25f }, { 1.0f, 0.0f, 1.0f, 1.0f } },
        { {  -0.25f, -0.25f,  -0.25f }, { 1.0f, 1.0f, 1.0f, 1.0f } },
        { {  -0.25f,  -0.25f,  0.25f }, { 0.0f, 0.0f, 0.0f, 1.0f } },
        { { 0.25f,  -0.25f,  0.25f }, { 1.0f, 1.0f, 0.0f, 1.0f } },
	};

	static constexpr auto CubeVertexBufferSize = sizeof(CubeVerticesPosCol);

    const std::array<u16, 36> CubeIndices
    {
        0, 1, 2,
        0, 2, 3,
        0, 4, 5,
        0, 5, 1,
        1, 5, 6,
		1, 6, 2,
		2, 6, 7,
		2, 7, 3,
		3, 7, 4,
		3, 4, 0,
		4, 7, 6,
		4, 6, 5
    };

	static const auto CubeIndexBufferSize = sizeof(CubeIndices);
}
