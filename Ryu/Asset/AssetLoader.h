#pragma once
#include "Asset/AssetData.h"
#include <filesystem>
#include <functional>
#include <optional>

namespace Ryu::Asset
{
	namespace fs = std::filesystem;

	template<typename T>
	using LoadFunc = std::function<std::optional<std::unique_ptr<T>>(const fs::path&)>;

	// Loader registry for different file types
	template<typename T>
	class AssetLoaderRegistry
	{
	public:
		void Register(const std::string& extension, LoadFunc<T> loader)
		{
			m_loaders[extension] = std::move(loader);
		}

		[[nodiscard]] std::optional<std::unique_ptr<T>> Load(const fs::path& path) const
		{
			const std::string ext = path.extension().string();

			if (auto it = m_loaders.find(ext); it != m_loaders.end())
			{
				return it->second(path);
			}

			return std::nullopt;
		}

		[[nodiscard]] bool CanLoad(const fs::path& path) const
		{
			return m_loaders.contains(path.extension().string());
		}

	private:
		std::unordered_map<std::string, LoadFunc<T>> m_loaders;
	};
}
