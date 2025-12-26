#pragma once
#include "Asset/AssetData.h"
#include <numbers>
#include <cmath>

namespace Ryu::Asset
{
    enum class PrimitiveType : u8
    {
        Triangle,
        Cube,
        Plane,
        Sphere,
        Cylinder,
        Cone,

        MAX_COUNT
    };

    namespace Primitives
    {
        inline std::unique_ptr<MeshData> CreateTriangle()
        {
            auto mesh = std::make_unique<MeshData>();

            mesh->Vertices =
            {
                // Position | Normal | UV | Color
                { { 0.0f,  1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.5f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f} },
                { { 1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} },
                { {-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} },
            };

            mesh->Indices = { 0, 1, 2 };

            mesh->SubMeshes.push_back({ 0, 3, 0 });

            return mesh;
        }

        inline std::unique_ptr<MeshData> CreateCube()
        {
            auto mesh = std::make_unique<MeshData>();

            // 24 vertices (4 per face for proper normals)
            mesh->Vertices = {
                // Front face (z = -1)
                { {-1.0f, -1.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} },
                { {-1.0f,  1.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f} },
                { { 1.0f,  1.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f} },
                { { 1.0f, -1.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} },

                // Back face (z = 1)
                { { 1.0f, -1.0f,  1.0f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} },
                { { 1.0f,  1.0f,  1.0f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f} },
                { {-1.0f,  1.0f,  1.0f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f} },
                { {-1.0f, -1.0f,  1.0f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} },

                // Top face (y = 1)
                { {-1.0f,  1.0f, -1.0f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} },
                { {-1.0f,  1.0f,  1.0f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f} },
                { { 1.0f,  1.0f,  1.0f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f} },
                { { 1.0f,  1.0f, -1.0f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} },

                // Bottom face (y = -1)
                { {-1.0f, -1.0f,  1.0f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} },
                { {-1.0f, -1.0f, -1.0f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f} },
                { { 1.0f, -1.0f, -1.0f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f} },
                { { 1.0f, -1.0f,  1.0f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} },

                // Right face (x = 1)
                { { 1.0f, -1.0f, -1.0f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} },
                { { 1.0f,  1.0f, -1.0f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f} },
                { { 1.0f,  1.0f,  1.0f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f} },
                { { 1.0f, -1.0f,  1.0f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} },

                // Left face (x = -1)
                { {-1.0f, -1.0f,  1.0f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} },
                { {-1.0f,  1.0f,  1.0f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f} },
                { {-1.0f,  1.0f, -1.0f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f} },
                { {-1.0f, -1.0f, -1.0f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} },
            };

            mesh->Indices = {
                0,  1,  2,   0,  2,  3,   // Front
                4,  5,  6,   4,  6,  7,   // Back
                8,  9,  10,  8,  10, 11,  // Top
                12, 13, 14,  12, 14, 15,  // Bottom
                16, 17, 18,  16, 18, 19,  // Right
                20, 21, 22,  20, 22, 23   // Left
            };

            mesh->SubMeshes.push_back({ 0, 36, 0 });

            return mesh;
        }

        inline std::unique_ptr<MeshData> CreatePlane(u32 subdivisions = 1)
        {
            auto mesh = std::make_unique<MeshData>();

            const u32 verticesPerSide = subdivisions + 1;
            const f32 step = 2.0f / static_cast<f32>(subdivisions);

            // Generate vertices
            for (u32 z = 0; z < verticesPerSide; ++z)
            {
                for (u32 x = 0; x < verticesPerSide; ++x)
                {
                    const f32 px = -1.0f + static_cast<f32>(x) * step;
                    const f32 pz = -1.0f + static_cast<f32>(z) * step;
                    const f32 u  = static_cast<f32>(x) / static_cast<f32>(subdivisions);
                    const f32 v  = static_cast<f32>(z) / static_cast<f32>(subdivisions);

                    mesh->Vertices.push_back(
                    {
                        { px, 0.0f, pz           },
                        { 0.0f, 1.0f, 0.0f       },
                        { u, v                   },
                        { 1.0f, 1.0f, 1.0f, 1.0f }
                    });
                }
            }

            // Generate indices
            for (u32 z = 0; z < subdivisions; ++z)
            {
                for (u32 x = 0; x < subdivisions; ++x)
                {
                    const u32 topLeft     = z * verticesPerSide + x;
                    const u32 topRight    = topLeft + 1;
                    const u32 bottomLeft  = (z + 1) * verticesPerSide + x;
                    const u32 bottomRight = bottomLeft + 1;

                    mesh->Indices.push_back(topLeft);
                    mesh->Indices.push_back(bottomLeft);
                    mesh->Indices.push_back(topRight);

                    mesh->Indices.push_back(topRight);
                    mesh->Indices.push_back(bottomLeft);
                    mesh->Indices.push_back(bottomRight);
                }
            }

            mesh->SubMeshes.push_back({ 0, static_cast<u32>(mesh->Indices.size()), 0 });

            return mesh;
        }

        inline std::unique_ptr<MeshData> CreateSphere(u32 slices = 32, u32 stacks = 16)
        {
            auto mesh = std::make_unique<MeshData>();

            constexpr f32 pi = std::numbers::pi_v<f32>;

            // Generate vertices
            for (u32 stack = 0; stack <= stacks; ++stack)
            {
                const f32 phi = pi * static_cast<f32>(stack) / static_cast<f32>(stacks);
                const f32 sinPhi = std::sin(phi);
                const f32 cosPhi = std::cos(phi);

                for (u32 slice = 0; slice <= slices; ++slice)
                {
                    const f32 theta = 2.0f * pi * static_cast<f32>(slice) / static_cast<f32>(slices);
                    const f32 sinTheta = std::sin(theta);
                    const f32 cosTheta = std::cos(theta);

                    const f32 x = cosTheta * sinPhi;
                    const f32 y = cosPhi;
                    const f32 z = sinTheta * sinPhi;

                    const f32 u = static_cast<f32>(slice) / static_cast<f32>(slices);
                    const f32 v = static_cast<f32>(stack) / static_cast<f32>(stacks);

                    mesh->Vertices.push_back(
                    {
                        { x, y, z               },
                        { x, y, z               },  // Normal = position for unit sphere
                        { u, v                  },
                        { 1.0f, 1.0f, 1.0f, 1.0f}
                    });
                }
            }

            // Generate indices
            for (u32 stack = 0; stack < stacks; ++stack)
            {
                for (u32 slice = 0; slice < slices; ++slice)
                {
                    const u32 first = stack * (slices + 1) + slice;
                    const u32 second = first + slices + 1;

                    mesh->Indices.push_back(first);
                    mesh->Indices.push_back(second);
                    mesh->Indices.push_back(first + 1);

                    mesh->Indices.push_back(second);
                    mesh->Indices.push_back(second + 1);
                    mesh->Indices.push_back(first + 1);
                }
            }

            mesh->SubMeshes.push_back({ 0, static_cast<u32>(mesh->Indices.size()), 0 });

            return mesh;
        }

        inline std::unique_ptr<MeshData> CreateCylinder(u32 slices = 32, f32 height = 2.0f, f32 radius = 1.0f)
        {
            auto mesh = std::make_unique<MeshData>();

            constexpr f32 pi = std::numbers::pi_v<f32>;
            const f32 halfHeight = height * 0.5f;

            // Side vertices
            for (u32 i = 0; i <= slices; ++i)
            {
                const f32 angle = 2.0f * pi * static_cast<f32>(i) / static_cast<f32>(slices);
                const f32 x     = std::cos(angle) * radius;
                const f32 z     = std::sin(angle) * radius;
                const f32 u     = static_cast<f32>(i) / static_cast<f32>(slices);

                // Bottom
                mesh->Vertices.push_back(
                {
                    { x, -halfHeight, z                      },
                    { std::cos(angle), 0.0f, std::sin(angle) },
                    { u, 1.0f                                },
                    { 1.0f, 1.0f, 1.0f, 1.0f                 }
                });

                // Top
                mesh->Vertices.push_back(
                {
                    { x, halfHeight, z                       },
                    { std::cos(angle), 0.0f, std::sin(angle) },
                    { u, 0.0f                                },
                    { 1.0f, 1.0f, 1.0f, 1.0f                 }
                });
            }

            // Side indices
            for (u32 i = 0; i < slices; ++i)
            {
                const u32 bottom = i * 2;
                const u32 top = bottom + 1;
                const u32 nextBottom = bottom + 2;
                const u32 nextTop = bottom + 3;

                mesh->Indices.push_back(bottom);
                mesh->Indices.push_back(top);
                mesh->Indices.push_back(nextBottom);

                mesh->Indices.push_back(nextBottom);
                mesh->Indices.push_back(top);
                mesh->Indices.push_back(nextTop);
            }

            // Top cap center
            const u32 topCenterIdx = static_cast<u32>(mesh->Vertices.size());
            mesh->Vertices.push_back(
            {
                {0.0f, halfHeight, 0.0f },
                {0.0f, 1.0f, 0.0f       },
                {0.5f, 0.5f             },
                {1.0f, 1.0f, 1.0f, 1.0f }
            });

            // Bottom cap center
            const u32 bottomCenterIdx = static_cast<u32>(mesh->Vertices.size());
            mesh->Vertices.push_back(
            {
                {0.0f, -halfHeight, 0.0f },
                {0.0f, -1.0f, 0.0f       },
                {0.5f, 0.5f              },
                {1.0f, 1.0f, 1.0f, 1.0f  }
            });

            // Cap vertices (separate for different normals)
            const u32 topCapStart    = static_cast<u32>(mesh->Vertices.size());
            const u32 bottomCapStart = topCapStart + slices + 1;

            for (u32 i = 0; i <= slices; ++i)
            {
                const f32 angle = 2.0f * pi * static_cast<f32>(i) / static_cast<f32>(slices);
                const f32 x = std::cos(angle) * radius;
                const f32 z = std::sin(angle) * radius;

                // Top cap
                mesh->Vertices.push_back(
                {
                    { x, halfHeight, z                 },
                    { 0.0f, 1.0f, 0.0f                 },
                    { x * 0.5f + 0.5f, z * 0.5f + 0.5f },
                    { 1.0f, 1.0f, 1.0f, 1.0f           }
                });

                // Bottom cap
                mesh->Vertices.push_back(
                {
                    { x, -halfHeight, z                },
                    { 0.0f, -1.0f, 0.0f                },
                    { x * 0.5f + 0.5f, z * 0.5f + 0.5f },
                    { 1.0f, 1.0f, 1.0f, 1.0f           }
                });
            }

            // Cap indices
            for (u32 i = 0; i < slices; ++i)
            {
                // Top cap
                mesh->Indices.push_back(topCenterIdx);
                mesh->Indices.push_back(topCapStart + i);
                mesh->Indices.push_back(topCapStart + i + 1);

                // Bottom cap
                mesh->Indices.push_back(bottomCenterIdx);
                mesh->Indices.push_back(bottomCapStart + i + 1);
                mesh->Indices.push_back(bottomCapStart + i);
            }

            mesh->SubMeshes.push_back({ 0, static_cast<u32>(mesh->Indices.size()), 0 });

            return mesh;
        }

        inline std::unique_ptr<MeshData> CreateCone(u32 slices = 32, f32 height = 2.0f, f32 radius = 1.0f)
        {
            auto mesh = std::make_unique<MeshData>();

            constexpr f32 pi = std::numbers::pi_v<f32>;
            const f32 halfHeight = height * 0.5f;

            // Apex
            const u32 apexIdx = 0;
            mesh->Vertices.push_back(
            {
                { 0.0f, halfHeight, 0.0f },
                { 0.0f, 1.0f, 0.0f       },
                { 0.5f, 0.0f             },
                { 1.0f, 1.0f, 1.0f, 1.0f }
            });

            // Side vertices
            for (u32 i = 0; i <= slices; ++i)
            {
                const f32 angle = 2.0f * pi * static_cast<f32>(i) / static_cast<f32>(slices);
                const f32 x = std::cos(angle) * radius;
                const f32 z = std::sin(angle) * radius;

                // Calculate normal for cone surface
                const f32 nx = std::cos(angle);
                const f32 nz = std::sin(angle);
                const f32 ny = radius / height;
                const f32 len = std::sqrt(nx * nx + ny * ny + nz * nz);

                mesh->Vertices.push_back(
                {
                    { x, -halfHeight, z                                    },
                    { nx / len, ny / len, nz / len                         },
                    { static_cast<f32>(i) / static_cast<f32>(slices), 1.0f },
                    { 1.0f, 1.0f, 1.0f, 1.0f                               }
                });
            }

            // Side indices
            for (u32 i = 0; i < slices; ++i)
            {
                mesh->Indices.push_back(apexIdx);
                mesh->Indices.push_back(i + 1);
                mesh->Indices.push_back(i + 2);
            }

            // Bottom cap center
            const u32 bottomCenterIdx = static_cast<u32>(mesh->Vertices.size());
            mesh->Vertices.push_back(
            {
                { 0.0f, -halfHeight, 0.0f },
                { 0.0f, -1.0f, 0.0f       },
                { 0.5f, 0.5f              },
                { 1.0f, 1.0f, 1.0f, 1.0f  }
            });

            // Bottom cap vertices
            const u32 bottomCapStart = static_cast<u32>(mesh->Vertices.size());
            for (u32 i = 0; i <= slices; ++i)
            {
                const f32 angle = 2.0f * pi * static_cast<f32>(i) / static_cast<f32>(slices);
                const f32 x = std::cos(angle) * radius;
                const f32 z = std::sin(angle) * radius;

                mesh->Vertices.push_back(
                {
                    { x, -halfHeight, z                },
                    { 0.0f, -1.0f, 0.0f                },
                    { x * 0.5f + 0.5f, z * 0.5f + 0.5f },
                    { 1.0f, 1.0f, 1.0f, 1.0f           }
                });
            }

            // Bottom cap indices
            for (u32 i = 0; i < slices; ++i)
            {
                mesh->Indices.push_back(bottomCenterIdx);
                mesh->Indices.push_back(bottomCapStart + i + 1);
                mesh->Indices.push_back(bottomCapStart + i);
            }

            mesh->SubMeshes.push_back({ 0, static_cast<u32>(mesh->Indices.size()), 0 });

            return mesh;
        }
    }
}
