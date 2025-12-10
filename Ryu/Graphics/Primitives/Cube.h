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
        // Front face (CCW when viewed from front)
        0, 1, 2, 0, 2, 3,
        // Right face
        1, 5, 6, 1, 6, 2,
        // Back face
        5, 4, 7, 5, 7, 6,
        // Left face
        4, 0, 3, 4, 3, 7,
        // Top face
        3, 2, 6, 3, 6, 7,
        // Bottom face
        4, 5, 1, 4, 1, 0
    };

	static const auto CubeIndexBufferSize = sizeof(CubeIndices);
}
