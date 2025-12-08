#include "ResourceManager.h"
#include "Asset/Loaders/ImageLoader.h"

namespace Ryu::Asset
{
	ResourceManager::ResourceManager()
	{
		m_textureManager.RegisterLoader(".jpg", { JPGLoader::Load, JPGLoader::Unload });
		m_textureManager.RegisterLoader(".png", { PNGLoader::Load, PNGLoader::Unload });
		m_textureManager.RegisterLoader(".bmp", { BMPLoader::Load, BMPLoader::Unload });
		m_textureManager.RegisterLoader(".hdr", { HDRLoader::Load, HDRLoader::Unload });
	}
	
	AssetPtr<Texture> ResourceManager::CreateTexture(const fs::path& path, bool lazyLoad)
	{
		return AssetPtr<Texture>(&m_textureManager, m_textureManager.Load(path, lazyLoad));
	}
}
