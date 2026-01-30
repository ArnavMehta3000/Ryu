#include "AssetCache.h"
#include "Core/Logging/Logger.h"
#include "Core/Profiling/Profiling.h"

namespace Ryu::Asset
{
	template<typename TAssetData, typename TGpuResource>
	inline AssetHandle<TAssetData> AssetCache<TAssetData, TGpuResource>::Register(const fs::path& path)
	{
		RYU_PROFILE_SCOPE();
		std::unique_lock lock(m_mutex);

		// Check if already registered
		if (auto it = m_pathToId.find(path); it != m_pathToId.end())
		{
			return AssetHandle<TAssetData>{ it->second };
		}

		// Can we load this type?
		if (!m_loaderRegistry.CanLoad(path))
		{
			RYU_LOG_WARN("Asset {} cannot be loaded! File extension ({}) does not have a registered loader", path.string(), path.extension().string());
			return AssetHandle<TAssetData>{};
		}

		const AssetId id = GenerateId(path);

		Entry entry
		{
			.SourcePath   = path,
			.State        = AssetState::Unloaded,
			.IsProcedural = false
		};

		m_entries[id] = std::move(entry);
		m_pathToId[path] = id;

		return AssetHandle<TAssetData>{ id };
	}

	template<typename TAssetData, typename TGpuResource>
    inline AssetHandle<TAssetData> AssetCache<TAssetData, TGpuResource>::Register(AssetId id, std::unique_ptr<TAssetData> data, std::string_view name)
    {
		RYU_PROFILE_SCOPE();
    	std::unique_lock lock(m_mutex);

		// Check if already registered
		if (m_entries.contains(id))
		{
			return AssetHandle<TAssetData>{ id };
		}

		Entry entry
		{
		   .CpuData      = std::move(data),
		   .SourcePath   = name,
		   .State        = AssetState::Loaded,  // CPU data already provided
		   .IsProcedural = true,
		};

		m_entries[id] = std::move(entry);

		return AssetHandle<TAssetData>{ id };
    }

	template<typename TAssetData, typename TGpuResource>
	inline AssetHandle<TAssetData> AssetCache<TAssetData, TGpuResource>::Register(std::string_view name, std::unique_ptr<TAssetData> data)
	{
		RYU_PROFILE_SCOPE();
		const AssetId id = std::hash<std::string_view>{}(name);
		return Register(id, std::move(data), name);
	}

	template<typename TAssetData, typename TGpuResource>
	inline TGpuResource* AssetCache<TAssetData, TGpuResource>::GetGpu(AssetHandle<TAssetData> handle)
	{
		RYU_PROFILE_SCOPE();
		if (!handle.IsValid())
		{
			RYU_LOG_WARN("Invalid asset handle");
			return nullptr;
		}

		std::unique_lock lock(m_mutex);

		Entry* entry = FindEntry(handle.id);
		if (!entry)
		{
			return nullptr;
		}
		
		const std::string name = entry->SourcePath.filename().string();

		// Return if ready
		if (entry->State == AssetState::Ready && entry->GpuResource)
		{
			return entry->GpuResource.get();
		}

		// Check we need to load the CPU side data first
		if (entry->State == AssetState::Unloaded)
		{
			RYU_LOG_DEBUG("Loading CPU data for asset {}", name);
			if (!LoadCpuData(*entry))
			{
				RYU_LOG_ERROR("Failed to load CPU data for asset {}", name);
				entry->State = AssetState::Failed;
				return nullptr;
			}
		}

		// Create GPU resource if needed
		if (entry->State == AssetState::Loaded && entry->CpuData)
		{
			RYU_LOG_DEBUG("Creating GPU resource for asset {}", name);
			if (!CreateGpuResource(*entry, name))
			{
				RYU_LOG_ERROR("Failed to create GPU resource for asset {}", name);
				entry->State = AssetState::Failed;
				return nullptr;
			}
		}

		return entry->GpuResource.get();
	}

	template<typename TAssetData, typename TGpuResource>
	inline const TAssetData* AssetCache<TAssetData, TGpuResource>::GetCpu(AssetHandle<TAssetData> handle)
	{
		RYU_PROFILE_SCOPE();

		if (!handle.IsValid())
		{
			return nullptr;
		}

		std::unique_lock lock(m_mutex);

		Entry* entry = FindEntry(handle.id);
		if (!entry)
		{
			return nullptr;
		}

		// Load if needed
		if (entry->State == AssetState::Unloaded)
		{
			RYU_LOG_DEBUG("Loading CPU data for asset {}", entry->SourcePath.filename().string());
			
			if (!LoadCpuData(*entry))
			{
				RYU_LOG_ERROR("Failed to load CPU data for asset {}", entry->SourcePath.filename().string());
				entry->State = AssetState::Failed;
				return nullptr;
			}
		}

		return entry->CpuData.get();
	}

	template<typename TAssetData, typename TGpuResource>
	inline AssetState AssetCache<TAssetData, TGpuResource>::GetState(AssetHandle<TAssetData> handle) const
	{
		RYU_PROFILE_SCOPE();
		std::shared_lock lock(m_mutex);

		if (const Entry* entry = FindEntry(handle.id))
		{
			return entry->State;
		}

		return AssetState::Failed;
	}

	template<typename TAssetData, typename TGpuResource>
	inline void AssetCache<TAssetData, TGpuResource>::AddRef(AssetHandle<TAssetData> handle)
	{
		RYU_PROFILE_SCOPE();
		std::unique_lock lock(m_mutex);

		if (Entry* entry = FindEntry(handle.id))
		{
			++entry->RefCount;
		}
	}

	template<typename TAssetData, typename TGpuResource>
	inline void AssetCache<TAssetData, TGpuResource>::Release(AssetHandle<TAssetData> handle)
	{
		RYU_PROFILE_SCOPE();
		std::unique_lock lock(m_mutex);

		Entry* entry = FindEntry(handle.id);
		if (!entry || entry->RefCount == 0)
			return;

		--entry->RefCount;

		RYU_TODO("Unload when refcount hits zero")
	}

	template<typename TAssetData, typename TGpuResource>
	inline void AssetCache<TAssetData, TGpuResource>::Invalidate(AssetHandle<TAssetData> handle)
	{
		RYU_PROFILE_SCOPE();
		std::unique_lock lock(m_mutex);

		Entry* entry = FindEntry(handle.id);
		if (!entry || entry->IsProcedural)
		{
			return;
		}

		// Clear GPU resource, keep CPU data option for reload
		entry->GpuResource.reset();
		entry->CpuData.reset();
		entry->State = AssetState::Unloaded;
	}

	template<typename TAssetData, typename TGpuResource>
	inline void AssetCache<TAssetData, TGpuResource>::InvalidateAll()
	{
		RYU_PROFILE_SCOPE();
		std::unique_lock lock(m_mutex);

		for (auto& [id, entry] : m_entries)
		{
			if (!entry.IsProcedural)
			{
				entry.GpuResource.reset();
				entry.CpuData.reset();
				entry.State = AssetState::Unloaded;
			}
		}
	}

	template<typename TAssetData, typename TGpuResource>
	inline void AssetCache<TAssetData, TGpuResource>::RegisterLoader(const std::string& extension, LoadFunc<TAssetData> loader)
	{
		RYU_PROFILE_SCOPE();
		RYU_LOG_DEBUG("Registering loader for extension {}", extension);
		m_loaderRegistry.Register(extension, std::move(loader));
	}

	template<typename TAssetData, typename TGpuResource>
	inline typename AssetCache<TAssetData, TGpuResource>::Entry* AssetCache<TAssetData, TGpuResource>::FindEntry(AssetId id)
	{
		RYU_PROFILE_SCOPE();
		auto it = m_entries.find(id);
		return it != m_entries.end() ? &it->second : nullptr;
	}

	template<typename TAssetData, typename TGpuResource>
	inline bool AssetCache<TAssetData, TGpuResource>::LoadCpuData(Entry& entry)
	{
		RYU_PROFILE_SCOPE();
		if (entry.IsProcedural)
		{
			return entry.CpuData != nullptr;
		}

		entry.State = AssetState::Loading;

		auto result = m_loaderRegistry.Load(entry.SourcePath);
		if (!result.has_value())
		{
			entry.State = AssetState::Failed;
			return false;
		}

		entry.CpuData = std::move(result.value());
		entry.State = AssetState::Loaded;
		return true;
	}

	template<typename TAssetData, typename TGpuResource>
	bool AssetCache<TAssetData, TGpuResource>::CreateGpuResource(Entry& entry, std::string_view name)
	{
		RYU_PROFILE_SCOPE();
		if (!m_gpuFactory || !entry.CpuData)
		{
			return false;
		}

		// This requires specialization per asset type
		// See GpuResourceFactory implementation
		if constexpr (IsSame<TAssetData, MeshData>)
		{
			//entry.GpuResource = m_gpuFactory->CreateMesh(*entry.CpuData, name);
		}
		else if constexpr (IsSame<TAssetData, TextureData>)
		{
			//entry.GpuResource = m_gpuFactory->CreateTexture(*entry.CpuData, name);
		}

		if (entry.GpuResource)
		{
			entry.State = AssetState::Ready;
			return true;
		}

		return false;
	}
}
