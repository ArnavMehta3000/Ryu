#pragma once
#include "Asset/Primitives.h"
#include "Asset/AssetCache.h"

namespace Ryu::Gfx
{
	class Mesh;
	class Texture;
}

namespace Ryu::Asset
{
	using MeshCache = AssetCache<MeshData, Gfx::Mesh>;
	using TextureCache = AssetCache<TextureData, Gfx::Texture>;

	class AssetRegistry
	{
		RYU_DISABLE_COPY_AND_MOVE(AssetRegistry)
	public:
		explicit AssetRegistry(IGpuResourceFactory* gpuFactory);
		~AssetRegistry() = default;

		[[nodiscard]] inline MeshCache& Meshes() { return m_meshCache; }
		[[nodiscard]] inline TextureCache& Textures() { return m_textureCache; }

		[[nodiscard]] MeshHandle GetPrimitive(PrimitiveType type) const;
		[[nodiscard]] Gfx::Mesh* GetPrimitiveGpu(PrimitiveType type);

		void LoadAll();  // Force load
		void InvalidateAll();

	private:
		void RegisterPrimitives();

	private:
		MeshCache m_meshCache;
		TextureCache m_textureCache;
		std::array<MeshHandle, static_cast<u64>(PrimitiveType::MAX_COUNT)> m_primitives{};
	};
}
