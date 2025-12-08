#include "ResourceManager.h"
#include "Asset/Loaders/ImageLoader.h"
#include "Asset/Loaders/OBJLoader.h"

namespace Ryu::Asset
{
	ResourceManager::ResourceManager()
	{
		m_textureManager.RegisterLoader(".jpg", { JPGLoader::Load, JPGLoader::Unload });
		m_textureManager.RegisterLoader(".png", { PNGLoader::Load, PNGLoader::Unload });
		m_textureManager.RegisterLoader(".bmp", { BMPLoader::Load, BMPLoader::Unload });
		m_textureManager.RegisterLoader(".hdr", { HDRLoader::Load, HDRLoader::Unload });

		m_meshManager.RegisterLoader(".obj", { OBJLoader::Load, OBJLoader::Unload });
	}
	
	AssetPtr<Texture> ResourceManager::CreateTexture(const fs::path& path, bool lazyLoad)
	{
		return AssetPtr<Texture>(&m_textureManager, m_textureManager.Load(path, lazyLoad));
	}
	
	AssetPtr<Mesh> ResourceManager::CreateMesh(const fs::path& path, bool lazyLoad)
	{
		return AssetPtr<Mesh>(&m_meshManager, m_meshManager.Load(path, lazyLoad));
	}
}
