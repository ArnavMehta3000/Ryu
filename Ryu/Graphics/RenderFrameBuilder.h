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
	// Lightweight builder class that traverses the world and builds a render frame that can be consumed by the WorldRenderer
	class RenderFrameBuilder
	{
	public:
		struct Config
		{
			u32 ScreenWidth = 1920;
			u32 ScreenHeight = 1080;
		};

	public:
		RenderFrameBuilder() {}

		RenderFrameBuilder& SetAssetRegistry(Asset::AssetRegistry* registry);
		RenderFrameBuilder& SetScreenSize(u32 width, u32 height);

		[[nodiscard]] RenderFrame ExtractRenderData(Game::World& world, const Utils::FrameTimer& timer);
		[[nodiscard]] RenderView ExtractViewForCamera(Game::World& world, const CameraData& camera);

		[[nodiscard]] u32 GetScreenWidth() const { return m_config.ScreenWidth; }
		[[nodiscard]] u32 GetScreenHeight() const { return m_config.ScreenHeight; }
		[[nodiscard]] f32 GetAspectRatio() const;

	private:
		void CollectRenderables(Game::World& world,u32 cullingMask,
			std::vector<RenderItem>& opaqueOut, std::vector<RenderItem>& transparentOut);

		void CollectCameras(Game::World& world, std::vector<CameraData>& camerasOut);
		CameraData ExtractCameraData(const Game::Transform& transform, const Game::Camera& camera);
		RenderItem CreateRenderItem(const Game::Transform& transform, const Game::MeshRenderer& renderer);
		static u64 ComputeSortKey(const RenderItem& item);

	private:
		Asset::AssetRegistry*      m_assetRegistry = nullptr;
		RenderFrameBuilder::Config m_config{};
	};
}
