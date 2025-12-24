#pragma once
#include "Core/Common/StandardTypes.h"
#include <vector>
#include <string>
#include <array>

namespace Ryu::Asset
{
	struct Mesh
	{
		struct Material
		{
			std::string Name;
			std::array<f32, 3> Ambient;       // Ka
			std::array<f32, 3> Diffuse;       // Kd
			std::array<f32, 3> Specular;      // Ks
			std::array<f32, 3> Transmittance; // Kt
			std::array<f32, 3> Emission;      // Ke
			f32 Shininess;                    // Ns
			f32 Ior;                          // Ni (index of refraction)
			f32 Dissolve;                     // d (transparency)
			i32 Illumination;                 // illum model
			
			std::string AmbientTextureName;
			std::string DiffuseTextureName;
			std::string SpecularTextureName;
			std::string NormalTextureName;
		};

		std::vector<f32> Vertices;   // Position(x, y, z)
		std::vector<f32> Normals;    // Normal(x, y, z)
		std::vector<f32> TexCoords;  // UV(x, y)
		std::vector<i32> VertexIndices;
		std::vector<i32> NormalIndices;
		std::vector<i32> TexCoordIndices;
		std::vector<i32> MaterialIds;  // Material ID per face
		std::vector<Material> Materials;
	};
}
