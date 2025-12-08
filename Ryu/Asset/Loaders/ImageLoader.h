#pragma once
#include "Asset/AssetLoader.h"

namespace Ryu::Asset
{
	struct Texture;

	class ImageLoader
	{
	public:
		static std::optional<std::unique_ptr<Texture>> Load(const fs::path& path);
		static void Unload(Texture* texture);
	};

	using JPGLoader = ImageLoader;
	using PNGLoader = ImageLoader;
	using BMPLoader = ImageLoader;
	using HDRLoader = ImageLoader;
}
