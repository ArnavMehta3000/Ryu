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
	public:
		using Loader = AssetLoader<T>;
		using Handle = AssetHandle<T>;

		void RegisterLoader(const std::string& extension, Loader loader) { m_loaders[extension] = loader; }
		
		Handle Load(fs::path& path)
		{
			// Already loaded
			if (auto it = m_pathToId.find(path); it != m_pathToId.end())
			{
				u64 id = it->second;
				return Handle{ id, m_idToAsset[id].Generation };
			}

			const std::string ext = path.extension().string();
			auto laoderIt = m_loaders.find(ext);

			// No loader for this extension
			if (loaderIt == m_loaders.end())
			{
				return Handle{};
			}

			const u64 id = m_nextId++;
			AssetEntry entry{ .Path = path };

			// TODO - catch failure?
			entry.Resource = loaderIt->second.Load(path);
			entry.State = AssetState::Ready;

			m_idToAsset[id] = std::move(entry);
			m_pathToId[path] = id;

			// First ever load -> generation 0
			return Handle{ id, 0 };
		}

	private:
		struct AssetEntry
		{
			std::unique_ptr<T> Resource;
			u32 Generation = 0;
			u32 RefCount = 0;
			fs::path Path;
			AssetState State = AssetState::Loading;
		};

		std::unordered_map<u64, AssetEntry>     m_idToAsset;
		std::unordered_map<fs::path, u64>       m_pathToId;
		std::unordered_map<std::string, Loader> m_loaders;
		u64                                     m_nextId{ 1 };
	};
}
