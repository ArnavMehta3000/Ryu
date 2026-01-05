#pragma once
#include "Core/Utils/Reflection.h"
#include "Asset/AssetData.h"

namespace Ryu::Asset { class AssetRegistry; enum class PrimitiveType : u8; }

namespace Ryu::Game
{
	struct MeshRenderer
	{
		RYU_ENABLE_REFLECTION(MeshRenderer)
	public:
		MeshRenderer();
		explicit MeshRenderer(Asset::AssetRegistry* registry);
		MeshRenderer(Asset::AssetRegistry* registry, Asset::PrimitiveType primitive);

		bool IsVisible = true;
		u8 RenderLayer = 0;
		Asset::MeshHandle MeshHandle;

	private:
		Asset::AssetRegistry* m_assetRegistry;
	};
}

RYU_REFLECTED_CLASS(
	::Ryu::Game::MeshRenderer,
	"MeshRenderer",
	RYU_CLASS_ATTRIB(::Ryu::Game::MeshRenderer, IsVisible),
	RYU_CLASS_ATTRIB(::Ryu::Game::MeshRenderer, MeshHandle)
)
