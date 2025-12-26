#pragma once

namespace Ryu::Asset
{
	// CPU-side mesh data - loadable from disk or generated procedurally
	struct MeshData
	{
		struct Vertex
		{
			std::array<f32, 3> Position;
			std::array<f32, 3> Normal;
			std::array<f32, 2> TexCoord;
			std::array<f32, 4> Color;
		};

		struct SubMesh
		{
			u32 IndexOffset = 0;
			u32 IndexCount = 0;
			u32 MaterialIndex = 0;
		};

		struct Material
		{
			std::string Name;
			std::array<f32, 4> Albedo{ 1.0f, 1.0f, 1.0f, 1.0f };
			f32 Metallic = 0.0f;
			f32 Roughness = 0.5f;

			std::string AlbedoTexturePath;
			std::string NormalTexturePath;
			std::string MetallicRoughnessPath;
		};

		std::vector<Vertex> Vertices;
		std::vector<u32> Indices;
		std::vector<SubMesh> SubMeshes;
		std::vector<Material> Materials;

		[[nodiscard]] u32 VertexStride() const { return sizeof(Vertex); }
        [[nodiscard]] u32 VertexBufferSize() const { return static_cast<u32>(Vertices.size() * sizeof(Vertex)); }
        [[nodiscard]] u32 IndexBufferSize() const { return static_cast<u32>(Indices.size() * sizeof(u32)); }
        [[nodiscard]] bool HasIndices() const { return !Indices.empty(); }
	};

	// CPU-side texture data
	struct TextureData
	{
		enum class Format : u8
		{
			R8,
			RG8,
			RGB8,
			RGBA8,
			R16F,
			RG16F,
			RGBA16F,
			R32F,
			RG32F,
			RGBA32F
		};

		i32 Width          = 0;
		i32 Height         = 0;
		i32 Depth          = 1;
		i32 MipLevels      = 1;
		Format PixelFormat = Format::RGBA8;
		std::vector<byte> Data;

		[[nodiscard]] u32 BytesPerPixel() const;
        [[nodiscard]] u32 RowPitch() const { return Width * BytesPerPixel(); }
        [[nodiscard]] u32 DataSize() const { return static_cast<u32>(Data.size()); }
	};

	inline u32 TextureData::BytesPerPixel() const
    {
        switch (PixelFormat)
        {
            case Format::R8:      return 1;
            case Format::RG8:     return 2;
            case Format::RGB8:    return 3;
            case Format::RGBA8:   return 4;
            case Format::R16F:    return 2;
            case Format::RG16F:   return 4;
            case Format::RGBA16F: return 8;
            case Format::R32F:    return 4;
            case Format::RG32F:   return 8;
            case Format::RGBA32F: return 16;
            default:              return 4;
        }
    }
}
