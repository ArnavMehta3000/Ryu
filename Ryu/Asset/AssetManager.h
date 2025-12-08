#pragma once
#include "Asset/AssetHandle.h"
#include "Asset/AssetLoader.h"
#include <unordered_map>
#include <vector>

namespace Ryu::Asset
{
	template<typename T>
	class AssetManager
	{
		struct AssetEntry
		{
			std::shared_ptr<T> Resource;
			u32 Generation = 0;
			u32 RefCount = 0;
			fs::path Path;
			AssetState State = AssetState::Unloaded;
			bool LazyLoad = false;
		};

	public:
		using Loader = AssetLoader<T>;
		using Handle = AssetHandle<T>;
		~AssetManager() = default;

		void RegisterLoader(const std::string& extension, Loader loader) { m_loaders[extension] = loader; }
		
		std::shared_ptr<T> Get(Handle handle);
		AssetState GetState(Handle handle);
		Handle Load(const fs::path& path, bool lazyLoad = true);
		Handle LoadImmediate(const fs::path& path) { return Load(path, false); }
		void Unload(Handle handle);

	private:
		void LoadAssetData(AssetEntry& entry);
		void UnloadAssetData(AssetEntry& entry);
		void AddRef(Handle handle);
		void RemoveRef(Handle handle);

	private:
		std::unordered_map<u64, AssetEntry>     m_idToAsset;
		std::unordered_map<fs::path, u64>       m_pathToId;
		std::unordered_map<std::string, Loader> m_loaders;
		u64                                     m_nextId{ 1 };
	};


	template<typename T>
	inline std::shared_ptr<T> AssetManager<T>::Get(Handle handle)
	{
		auto it = m_idToAsset.find(handle.Id);
		if (it == m_idToAsset.end() || it->second.Generation != handle.Generation)
		{
			return nullptr;
		}

		AssetEntry& entry = it->second;

		// Lazy load if needed
		if (entry.LazyLoad && entry.State == AssetState::Unloaded)
		{
			LoadAssetData(entry);
		}

		// Increment ref count on first successful access
		if (entry.State == AssetState::Ready && entry.Resource)
		{
			AddRef(handle);
			return entry.Resource;
		}

		return nullptr;
	}

	template<typename T>
	inline AssetState AssetManager<T>::GetState(Handle handle)
	{
		auto it = m_idToAsset.find(handle.Id);
		return it != m_idToAsset.end() ? it->second.State : AssetState::Failed;
	}

	template<typename T>
	inline AssetManager<T>::Handle AssetManager<T>::Load(const fs::path& path, bool lazyLoad)
	{
		// Already loaded/registered
		if (auto it = m_pathToId.find(path); it != m_pathToId.end())
		{
			u64 id = it->second;
			return Handle{ id, m_idToAsset[id].Generation };
		}

		const std::string ext = path.extension().string();
		auto loaderIt = m_loaders.find(ext);

		// No loader for this extension
		if (loaderIt == m_loaders.end())
		{
			return Handle{};
		}
		
		const u64 id = m_nextId++;
		
		AssetEntry entry
		{
			.Resource = nullptr,
			.Path     = path,
			.State    = lazyLoad ? AssetState::Unloaded : AssetState::Loading,
			.LazyLoad = lazyLoad
		};

		if (!lazyLoad)
		{
			LoadAssetData(entry);
		}

		m_idToAsset[id] = std::move(entry);
		m_pathToId[path] = id;

		return Handle{ id, 0 };  // First ever load means generation 0
	}
	
	template<typename T>
	inline void AssetManager<T>::Unload(Handle handle)
	{
		auto it = m_idToAsset.find(handle.Id);
		if (it != m_idToAsset.end())
		{
			RemoveRef(handle);
		}
	}

	template<typename T>
	inline void AssetManager<T>::LoadAssetData(AssetEntry& entry)
	{
		if (entry.State != AssetState::Unloaded)
		{
			return;
		}

		const std::string ext = entry.Path.extension().string();
		auto loaderIt = m_loaders.find(ext);

		if (loaderIt != m_loaders.end())
		{
			if (std::optional<std::unique_ptr<T>> loadResult = loaderIt->second.Load(entry.Path))
			{
				entry.Resource = std::move(loadResult.value());
				entry.State = AssetState::Ready;
			}
			else
			{
				entry.State = AssetState::Failed;
			}
		}
		else
		{
			entry.State = AssetState::Failed;
		}
	}

	template<typename T>
	inline void AssetManager<T>::UnloadAssetData(AssetEntry& entry)
	{
		if (!entry.Resource || entry.State != AssetState::Ready)
		{
			return;
		}

		const std::string ext = entry.Path.extension().string();
		auto loaderIt = m_loaders.find(ext);

		if (loaderIt != m_loaders.end() && loaderIt->second.Unload)
		{
			loaderIt->second.Unload(entry.Resource.get());
		}

		entry.Resource.reset();
		entry.State = entry.LazyLoad ? AssetState::Unloaded : AssetState::Failed;
	}

	template<typename T>
	inline void AssetManager<T>::AddRef(Handle handle)
	{
		auto it = m_idToAsset.find(handle.Id);
		if (it != m_idToAsset.end() && it->second.Generation == handle.Generation)
		{
			++it->second.RefCount;
		}
	}

	template<typename T>
	inline void AssetManager<T>::RemoveRef(Handle handle)
	{
		auto it = m_idToAsset.find(handle.Id);
		if (it == m_idToAsset.end() || it->second.Generation != handle.Generation)
		{
			return;
		}

		AssetEntry& entry = it->second;

		if (entry.RefCount > 0)
		{
			--entry.RefCount;
		}

		// Unload if ref count reaches zero
		if (entry.RefCount == 0)
		{
			UnloadAssetData(entry);
			// Could remove from map here but saving it for hot-reloading
		}
	}
}
