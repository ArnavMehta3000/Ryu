#pragma once
#include "Asset/AssetHandle.h"

namespace Ryu::Gfx
{
	class Mesh;
	class Texture;
}

namespace Ryu::Asset
{
	template <typename T> class AssetCache;
	struct MeshData;
	struct TextureData;

	template <typename TAssetData, typename TGpuResource>
	class AssetRef
	{
		RYU_DISABLE_COPY(AssetRef)
	public:
		AssetRef() = default;

		AssetRef(AssetCache<TAssetData, TGpuResource>* cache, AssetHandle<TAssetData> handle)
			: m_cache(cache)
			, m_handle(handle)
		{
			if (m_cache && m_handle.IsValid())
			{
				m_cache->AddRef(m_handle);
			}
		}

		~AssetRef()
		{
			Release();
		}

		AssetRef(AssetRef&& other) noexcept
			: m_cache(other.m_cache)
			, m_handle(other.m_handle)
		{
			other.m_cache = nullptr;
			other.m_handle = {};
		}

		AssetRef& operator=(AssetRef&& other) noexcept
		{
			if (this != &other)
			{
				Release();
				m_cache        = other.m_cache;
				m_handle       = other.m_handle;
				other.m_cache  = nullptr;
				other.m_handle = {};
			}
			return *this;
		}

		void Release()
		{
			if (m_cache && m_handle.IsValid())
			{
				m_cache->Release(m_handle);
			}
			m_cache = nullptr;
			m_handle = {};
		}

		[[nodiscard]] inline TGpuResource* GetGpu() const { return m_cache ? m_cache->GetGpu(m_handle) : nullptr; }
		[[nodiscard]] inline const TAssetData* GetCpu() const { return m_cache ? m_cache->GetCpu(m_handle) : nullptr; }

		[[nodiscard]] AssetHandle<TAssetData> GetHandle() const { return m_handle; }
		[[nodiscard]] bool IsValid() const { return m_handle.IsValid(); }
		explicit operator bool() const { return IsValid(); }

	private:
		AssetCache<TAssetData, TGpuResource>* m_cache = nullptr;
		AssetHandle<TAssetData> m_handle;
	};

	using MeshRef = AssetRef<MeshData, Gfx::Mesh>;
	using TextureRef = AssetRef<TextureData, Gfx::Texture>;
}
