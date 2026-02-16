#include "Asset/AssetRegistry.h"
#include "Asset/Loaders/ImageLoader.h"
#include "Asset/Loaders/OBJLoader.h"
#include "Core/Profiling/Profiling.h"

namespace Ryu::Asset
{
	AssetRegistry::AssetRegistry(IGpuResourceFactory* gpuFactory)
		: m_meshCache(gpuFactory)
		, m_textureCache(gpuFactory)
	{
		RYU_PROFILE_SCOPE();
		RegisterPrimitives();
	}

	MeshHandle AssetRegistry::GetPrimitive(PrimitiveType type) const
	{
		const u64 index = static_cast<u64>(type);
		if (index >= m_primitives.size())
		{
			return MeshHandle{};
		}

		return m_primitives[index];
	}

	Gfx::Mesh* AssetRegistry::GetPrimitiveGpu(PrimitiveType type)
	{
		RYU_PROFILE_SCOPE();
		return m_meshCache.GetGpu(GetPrimitive(type));
	}

	void AssetRegistry::LoadAll()
	{
		RYU_PROFILE_SCOPE();
		m_meshCache.ForEach([this](MeshHandle handle, const MeshCache::Entry&)
		{
			std::ignore = m_meshCache.GetGpu(handle);
		});

		m_textureCache.ForEach([this](TextureHandle handle, const TextureCache::Entry&)
		{
			std::ignore = m_textureCache.GetGpu(handle);
		});
	}

	void AssetRegistry::InvalidateAll()
	{
		RYU_PROFILE_SCOPE();
		m_meshCache.InvalidateAll();
		m_textureCache.InvalidateAll();
	}

	void AssetRegistry::RegisterPrimitives()
	{
		RYU_PROFILE_SCOPE();
		// Register all built-in primitives
		m_primitives[static_cast<u64>(PrimitiveType::Triangle)] = m_meshCache.Register("Primitive:Triangle", Primitives::CreateTriangle());
		m_primitives[static_cast<u64>(PrimitiveType::Cube)]     = m_meshCache.Register("Primitive:Cube", Primitives::CreateCube());
		m_primitives[static_cast<u64>(PrimitiveType::Plane)]    = m_meshCache.Register("Primitive:Plane", Primitives::CreatePlane());
		m_primitives[static_cast<u64>(PrimitiveType::Sphere)]   = m_meshCache.Register("Primitive:Sphere", Primitives::CreateSphere());
		m_primitives[static_cast<u64>(PrimitiveType::Cylinder)] = m_meshCache.Register("Primitive:Cylinder", Primitives::CreateCylinder());
		m_primitives[static_cast<u64>(PrimitiveType::Cone)]     = m_meshCache.Register("Primitive:Cone", Primitives::CreateCone());
	}
}
