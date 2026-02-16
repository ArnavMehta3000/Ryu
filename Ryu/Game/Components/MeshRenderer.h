#pragma once
#include "Core/Utils/Reflection.h"
#include "Asset/AssetData.h"

namespace Ryu::Asset { class AssetRegistry; enum class PrimitiveType : u8; }
namespace Ryu::Editor { class MeshRendererPanel; }
namespace Ryu::Engine { class Engine; }

namespace Ryu::Game
{
	struct MeshRenderer
	{
		friend class Editor::MeshRendererPanel;
		friend class Engine::Engine;
		static constexpr auto ComponentName = "Mesh Renderer";
		RYU_ENABLE_REFLECTION(MeshRenderer)

	public:
		bool IsVisible = true;
		u8 RenderLayer = 0;
		Asset::MeshHandle MeshHandle;

		[[nodiscard]] Asset::AssetRegistry* GetAssetRegistry() { return m_assetRegistry; }

	private:
		static inline Asset::AssetRegistry* m_assetRegistry;
	};
}

RYU_REFLECTED_CLASS(
	::Ryu::Game::MeshRenderer,
	"MeshRenderer",
	RYU_CLASS_ATTRIB(::Ryu::Game::MeshRenderer, IsVisible),
	RYU_CLASS_ATTRIB(::Ryu::Game::MeshRenderer, MeshHandle)
)
