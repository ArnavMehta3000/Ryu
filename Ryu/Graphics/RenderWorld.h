#pragma once
#include "Graphics/RenderData.h"

namespace Ryu::Game
{
	class World;
	struct Camera;
	struct Transform;
	struct MeshRenderer;
}
namespace Ryu::Asset { class AssetRegistry; }

namespace Ryu::Gfx
{
	class RenderWorld
	{
	public:
		explicit RenderWorld(Asset::AssetRegistry* registry);

		[[nodiscard]] RenderFrame ExtractRenderData(Game::World& world, f32 deltaTime, f32 TotalTime, u64 frameNumber);
		[[nodiscard]] RenderView ExtractViewForCamera(Game::World& world, const CameraData& camera);

	private:
		void CollectRenderables(Game::World& world, u32 cullingMask, std::vector<RenderItem>& opaqueOut, std::vector<RenderItem>& transparentOut);
		void CollectCameras(Game::World& world, std::vector<CameraData>& camerasOut);
		CameraData ExtractCameraData(const Game::Transform& transform, const Game::Camera& camera, u32 screenWidth, u32 screenHeight);
		RenderItem CreateRenderItem(const Game::Transform& transform, const Game::MeshRenderer& renderer);

		static u64 ComputeSortKey(const RenderItem& item);

	private:
		Asset::AssetRegistry* m_assetRegistry = nullptr;
	};
}
