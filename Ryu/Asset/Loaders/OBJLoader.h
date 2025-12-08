#pragma once
#include "Asset/AssetLoader.h"

namespace Ryu::Asset
{
	struct Mesh;

	class OBJLoader
	{
	public:
		static std::optional<std::unique_ptr<Mesh>> Load(const fs::path& path);
		static void Unload(Mesh* mesh);
	};
}
