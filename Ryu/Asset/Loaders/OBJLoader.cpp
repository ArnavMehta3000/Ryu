#include "Asset/Loaders/OBJLoader.h"
#include "Asset/Assets/MeshAsset.h"
#include "Logging/Logger.h"
#include <External/TinyOBJ/tiny_obj_loader.h>
#include <fstream>

namespace Ryu::Asset
{
	std::optional<std::unique_ptr<Mesh>> OBJLoader::Load(const fs::path& path)
	{
		std::ifstream ifs(path);

		if (ifs.fail())
		{
			return std::nullopt;
		}

		std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();
		std::string err;

		tinyobj::MaterialFileReader mtlReader(path.parent_path().string());

		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;

		// Taken from TinyOBJ README
		if (tinyobj::LoadObj(&attrib, &shapes, &materials, &err, &ifs, &mtlReader))
		{
			mesh->Vertices = attrib.vertices;
			mesh->Normals = attrib.normals;
			mesh->TexCoords = attrib.texcoords;

			// Convert materials
			for (const auto& mat : materials)
			{
				Mesh::Material m;
				m.Name                = mat.name;
				std::copy(std::begin(mat.ambient), std::end(mat.ambient), m.Ambient);
				std::copy(std::begin(mat.diffuse), std::end(mat.diffuse), m.Diffuse);
				std::copy(std::begin(mat.specular), std::end(mat.specular), m.Specular);
				std::copy(std::begin(mat.transmittance), std::end(mat.transmittance), m.Transmittance);
				std::copy(std::begin(mat.emission), std::end(mat.emission), m.Emission);
				m.Shininess           = mat.shininess;
				m.Ior                 = mat.ior;
				m.Dissolve            = mat.dissolve;
				m.Illumination        = mat.illum;
				m.AmbientTextureName  = mat.ambient_texname;
				m.DiffuseTextureName  = mat.diffuse_texname;
				m.SpecularTextureName = mat.specular_texname;
				m.NormalTextureName   = mat.bump_texname;

				mesh->Materials.push_back(m);
			}

			// Extract indices and material IDs
			for (const auto& shape : shapes)
			{
				for (size_t f = 0; f < shape.mesh.indices.size() / 3; ++f)
				{
					for (int v = 0; v < 3; ++v)
					{
						const auto& index = shape.mesh.indices[3 * f + v];
						mesh->VertexIndices.push_back(index.vertex_index);
						mesh->NormalIndices.push_back(index.normal_index);
						mesh->TexCoordIndices.push_back(index.texcoord_index);
					}
					mesh->MaterialIds.push_back(shape.mesh.material_ids[f]);
				}
			}

			return mesh;
		}
		else
		{
			std::nullopt;
		}
	}

	void OBJLoader::Unload(Mesh* mesh)
	{
		if (mesh)
		{
			mesh->Vertices.clear();
			mesh->Normals.clear();
			mesh->TexCoords.clear();
			mesh->VertexIndices.clear();
			mesh->NormalIndices.clear();
			mesh->TexCoordIndices.clear();
		}
	}
}
