#include "Game/Components/MeshRenderer.h"
#include "Asset/Primitives.h"
#include "Asset/AssetRegistry.h"

namespace Ryu::Game
{
	MeshRenderer::MeshRenderer()
		: m_assetRegistry(nullptr)
	{
	}

	MeshRenderer::MeshRenderer(Asset::AssetRegistry* registry)
		: m_assetRegistry(registry)
	{
	}

	MeshRenderer::MeshRenderer(Asset::AssetRegistry* registry, Asset::PrimitiveType primitive)
		: MeshHandle(registry->GetPrimitive(primitive))
		, m_assetRegistry(registry)
	{		
	}
}
