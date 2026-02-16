#include "Asset/Loaders/OBJLoader.h"
#include "Asset/AssetData.h"
#include "Core/Logging/Logger.h"
#include <TinyOBJ/tiny_obj_loader.h>
#include <fstream>

namespace Ryu::Asset
{
    std::unique_ptr<MeshData> OBJLoader::Load(const fs::path& path)
    {
        std::ifstream ifs(path);
        if (ifs.fail())
        {
            return nullptr;
        }

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string err;

        tinyobj::MaterialFileReader mtlReader(path.parent_path().string());

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, &ifs, &mtlReader))
        {
            return nullptr;
        }

        auto mesh = std::make_unique<MeshData>();

        // Convert materials
        for (const auto& mat : materials)
        {
            MeshData::Material m
            {
                .Name              = mat.name,
                .Albedo            = { mat.diffuse[0], mat.diffuse[1], mat.diffuse[2], 1.0f },
                .Metallic          = mat.metallic,
                .Roughness         = 1.0f - (mat.shininess / 1000.0f), // Approximate conversion
                .AlbedoTexturePath = mat.diffuse_texname,
                .NormalTexturePath = mat.bump_texname
            };
            mesh->Materials.push_back(m);
        }

        // Build interleaved vertices (unindex the mesh for GPU)
        //u32 indexOffset = 0;
        for (const auto& shape : shapes)
        {
            MeshData::SubMesh submesh;
            submesh.IndexOffset = static_cast<u32>(mesh->Indices.size());

            for (size_t f = 0; f < shape.mesh.indices.size(); ++f)
            {
                const auto& idx = shape.mesh.indices[f];

                MeshData::Vertex vertex{};

                // Position
                vertex.Position =
                {
                    attrib.vertices[3 * idx.vertex_index + 0],
                    attrib.vertices[3 * idx.vertex_index + 1],
                    attrib.vertices[3 * idx.vertex_index + 2]
                };

                // Normal
                if (idx.normal_index >= 0)
                {
                    vertex.Normal =
                    {
                        attrib.normals[3 * idx.normal_index + 0],
                        attrib.normals[3 * idx.normal_index + 1],
                        attrib.normals[3 * idx.normal_index + 2]
                    };
                }

                // TexCoord
                if (idx.texcoord_index >= 0)
                {
                    vertex.TexCoord =
                    {
                        attrib.texcoords[2 * idx.texcoord_index + 0],
                        attrib.texcoords[2 * idx.texcoord_index + 1]
                    };
                }

                vertex.Color = { 1.0f, 1.0f, 1.0f, 1.0f };

                mesh->Vertices.push_back(vertex);
                mesh->Indices.push_back(static_cast<u32>(mesh->Vertices.size() - 1));
            }

            submesh.IndexCount    = static_cast<u32>(mesh->Indices.size()) - submesh.IndexOffset;
            submesh.MaterialIndex = shape.mesh.material_ids.empty() ? 0 : shape.mesh.material_ids[0];

            mesh->SubMeshes.push_back(submesh);
        }

        return mesh;
    }
}
