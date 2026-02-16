#pragma once
#include "Asset/AssetHandle.h"
#include "Asset/AssetLoader.h"
#include "Asset/IGpuResourceFactory.h"
#include <mutex>
#include <shared_mutex>

namespace Ryu::Asset
{
	namespace fs = std::filesystem;

	// Manages CPU and GPU resources together + support for lazy loading
	template<typename TAssetData, typename TGpuResource>
	class AssetCache
	{
	public:
		struct Entry
		{
			std::unique_ptr<TAssetData>   CpuData;
			std::unique_ptr<TGpuResource> GpuResource;
			fs::path                      SourcePath;
			std::string                   Name;
			AssetState                    State = AssetState::Unloaded;
			u32                           RefCount = 0;
			bool                          IsProcedural = false;
		};

	public:
		explicit AssetCache(IGpuResourceFactory* gpuFactory) : m_gpuFactory(gpuFactory) {}

		// Register asset from file path (lazy loading)
		[[nodiscard]] AssetHandle<TAssetData> Register(const fs::path& path);

		// Register procedural asset with explicit ID and data
		[[nodiscard]] AssetHandle<TAssetData> Register(AssetId id, std::unique_ptr<TAssetData> data, std::string_view name = "");

		// Register procedural asset with string ID
		[[nodiscard]] AssetHandle<TAssetData> Register(std::string_view name, std::unique_ptr<TAssetData> data);

		// Get CPU/GPU data - loads on demand
		[[nodiscard]] TGpuResource* GetGpu(AssetHandle<TAssetData> handle);
		[[nodiscard]] const TAssetData* GetCpu(AssetHandle<TAssetData> handle);

		[[nodiscard]] const Entry* const GetEntry(AssetHandle<TAssetData> handle) const { return FindEntry(handle.Id); }

		// Force load without accessing
		void EnsureCpuLoaded(AssetHandle<TAssetData> handle);
		void EnsureGpuReady(AssetHandle<TAssetData> handle);

		void AddRef(AssetHandle<TAssetData> handle);
		void Release(AssetHandle<TAssetData> handle);

		[[nodiscard]] AssetState GetState(AssetHandle<TAssetData> handle) const;
		void Invalidate(AssetHandle<TAssetData> handle);
		void InvalidateAll();

		// Func is void(AssetHandle<TAssetData> handle, const Entry& entry)
		template<typename Func>
		inline void ForEach(Func&& func) const
		{
			std::shared_lock lock(m_mutex);
			for (const auto& [id, entry] : m_entries)
			{
				func(AssetHandle<TAssetData>{ id }, entry);
			}
		}

	private:
		[[nodiscard]] AssetId GenerateId(const fs::path& path) const;
		[[nodiscard]] AssetId GenerateId(std::string_view name) const;
		[[nodiscard]] Entry* FindEntry(AssetId id);
		[[nodiscard]] const Entry* FindEntry(AssetId id) const;

		bool LoadCpuData(Entry& entry);
		bool CreateGpuResource(Entry& entry, std::string_view name);

	private:
		mutable std::shared_mutex              m_mutex;
		std::unordered_map<AssetId, Entry>     m_entries;
		std::unordered_map<fs::path, AssetId>  m_pathToId;
		IGpuResourceFactory*                   m_gpuFactory;
	};
}

#include "Asset/AssetCache.inl"
