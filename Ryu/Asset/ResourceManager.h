#pragma once
#include "Asset/AssetPtr.h"
#include "Asset/AssetManager.h"
#include "Assets/TextureAsset.h"

namespace Ryu::Asset
{
	using TextureManager = AssetManager<Texture>;
	using TextureHandle = TextureManager::Handle;

	class ResourceManager
	{
	public:
		ResourceManager();
		~ResourceManager() = default;

		AssetPtr<Texture> CreateTexture(const fs::path& path, bool lazyLoad = true);

		//[[nodiscard]] inline TextureManager& GetTextureManager() { return m_textureManager; }

	private:
		TextureManager m_textureManager;
	};
}
