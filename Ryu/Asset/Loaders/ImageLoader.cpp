#include "Asset/Loaders/ImageLoader.h"
#include "Asset/Assets/TextureAsset.h"
#include "Logging/Logger.h"
#include <External/STB/stb_image.h>

namespace Ryu::Asset
{
	std::optional<std::unique_ptr<Texture>> ImageLoader::Load(const fs::path& path)
	{
		i32 width = 0, height = 0, channels = 0;
		::stbi_set_flip_vertically_on_load(true);

		RYU_LOG_TRACE("Trying to load image {}", path.string());

		if (byte* pixels = ::stbi_load(path.string().c_str(), &width, &height, &channels, 0))
		{
			// Copy data to vector
			u64 dataSize = static_cast<u64>(width * height * channels);
			std::vector<byte> data(pixels, pixels + dataSize);

			::stbi_image_free(pixels);

			return std::make_unique<Texture>(width, height, channels, std::move(data));
		}
		else
		{
			RYU_LOG_WARN("Failed to load image {}", path.string());
			return std::nullopt;
		}
	}
	
	void ImageLoader::Unload(Texture* texture)
	{
		if (texture)
		{
			texture->Data.clear();
		}
	}
}
