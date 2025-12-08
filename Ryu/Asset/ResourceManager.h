#pragma once
#include "Asset/AssetPtr.h"
#include "Asset/AssetManager.h"
#include "Assets/TextureAsset.h"
#include "Assets/MeshAsset.h"

namespace Ryu::Asset
{
	using TextureManager = AssetManager<Texture>;
	using TextureHandle = TextureManager::Handle;

	using MeshManager = AssetManager<Mesh>;
	using MeshHandle = MeshManager::Handle;

	class ResourceManager
	{
	public:
		ResourceManager();
		~ResourceManager() = default;

		AssetPtr<Texture> CreateTexture(const fs::path& path, bool lazyLoad = true);
		AssetPtr<Mesh> CreateMesh(const fs::path& path, bool lazyLoad = true);

		//[[nodiscard]] inline TextureManager& GetTextureManager() { return m_textureManager; }
		//[[nodiscard]] inline MeshManager& GetMeshManager() { return m_meshManager; }

	private:
		TextureManager m_textureManager;
		MeshManager m_meshManager;
	};
}
