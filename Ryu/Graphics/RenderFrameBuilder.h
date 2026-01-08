#pragma once
#include "Graphics/RenderData.h"

namespace Ryu::Game
{
	class World;
	struct CameraComponent;
	struct Transform;
	struct MeshRenderer;
}
namespace Ryu::Asset { class AssetRegistry; }
namespace Ryu::Utils { class FrameTimer; }

namespace Ryu::Gfx
{
	class Device;

	// Lightweight builder class that traverses the world and builds a render frame that can be consumed by the WorldRenderer
	class RenderFrameBuilder
	{
	public:
		RenderFrameBuilder(Asset::AssetRegistry* registry, Device* device)
			: m_assetRegistry(registry), m_device(device) {}

		[[nodiscard]] RenderFrame ExtractRenderData(Game::World& world, const Utils::FrameTimer& timer);
		[[nodiscard]] RenderView ExtractViewForCamera(Game::World& world, const CameraData& cameraData);

	private:
		void CollectRenderables(Game::World& world,u32 cullingMask,
			std::vector<RenderItem>& opaqueOut, std::vector<RenderItem>& transparentOut);

		void CollectCameras(Game::World& world, std::vector<CameraData>& camerasOut);
		CameraData ExtractCameraData(const Game::Transform& transform, const Game::CameraComponent& camera);
		RenderItem CreateRenderItem(const Game::Transform& transform, const Game::MeshRenderer& renderer);
		static u64 ComputeSortKey(const RenderItem& item);

	private:
		Device* m_device{ nullptr };
		Asset::AssetRegistry* m_assetRegistry{ nullptr };
	};
}
