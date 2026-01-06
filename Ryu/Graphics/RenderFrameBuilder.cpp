#include "Graphics/RenderFrameBuilder.h"

#include "Core/Profiling/Profiling.h"
#include "Game/Components/CameraComponent.h"
#include "Game/Components/MeshRenderer.h"
#include "Game/Components/TransformComponent.h"
#include "Game/World/World.h"
#include "Graphics/Camera.h"
#include <ranges>

namespace Ryu::Gfx
{
	RenderFrameBuilder& RenderFrameBuilder::SetAssetRegistry(Asset::AssetRegistry* registry)
	{
		m_assetRegistry = registry;
		return *this;
	}

	RenderFrameBuilder& RenderFrameBuilder::SetScreenSize(u32 width, u32 height)
	{
		m_config.ScreenWidth = width;
		m_config.ScreenHeight = height;
		return *this;
	}

	f32 RenderFrameBuilder::GetAspectRatio() const
	{
		return m_config.ScreenHeight > 0
			? static_cast<f32>(m_config.ScreenWidth) / static_cast<f32>(m_config.ScreenHeight)
			: 1.0f;
	}

	RenderFrame RenderFrameBuilder::ExtractRenderData(Game::World& world, const Utils::FrameTimer& timer)
	{
		RYU_PROFILE_SCOPE();

		RenderFrame frame
		{
			.DeltaTime   = timer.DeltaTimeF(),
			.TotalTime   = (f32)timer.TimeSinceStart<std::chrono::seconds>(),
			.FrameNumber = timer.FrameCount()
		};

		// Collect all cameras, sorted by priority
		std::vector<CameraData> cameras;
		CollectCameras(world, cameras);

		// If no camera components exist, SceneRenderer will use its default camera

		// For each camera, collect visible renderables
		for (const auto& camera : cameras)
		{
			RenderView view{};
			view.CameraData = camera;

			CollectRenderables(
				world,
				camera.CullingMask,
				view.OpaqueItems,
				view.TransparentItems);

			// Sort opaque front-to-back (minimize overdraw)
			std::ranges::sort(view.OpaqueItems,
			[](const RenderItem& a, const RenderItem& b)
			{
				return a.SortKey < b.SortKey;
			});

			// Sort transparent back-to-front (correct blending)
			std::ranges::sort(view.TransparentItems,
			[](const RenderItem& a, const RenderItem& b)
			{
				return a.SortKey > b.SortKey;
			});

			frame.Views.push_back(std::move(view));
		}

		return frame;
	}

	RenderView RenderFrameBuilder::ExtractViewForCamera(Game::World& world, const CameraData& camera)
	{
		RYU_PROFILE_SCOPE();

		RenderView view { .CameraData = camera };

		CollectRenderables(
			world,
			camera.CullingMask,
			view.OpaqueItems,
			view.TransparentItems);

		return view;
	}

	void RenderFrameBuilder::CollectRenderables(Game::World& world, u32 cullingMask, std::vector<RenderItem>& opaqueOut, 
		[[maybe_unused]] std::vector<RenderItem>& transparentOut)  // Temporary until I figure out transparency
	{
		RYU_PROFILE_SCOPE();

		auto& registry = world.GetRegistry();

		// Query all entities with Transform and MeshRenderer
		auto view = registry.view<Game::Transform, Game::MeshRenderer>();

		for (const auto& [entity, transform, renderer] : view.each())
		{
			if (!renderer.IsVisible                                      // Skip invisible
				|| ((cullingMask & (1u << renderer.RenderLayer)) == 0))  // Layer culling
			{
				continue;
			}

			RenderItem item = CreateRenderItem(transform, renderer);
			item.SortKey = ComputeSortKey(item);

			// For now, all items are opaque
			opaqueOut.push_back(item);
		}
	}

	void RenderFrameBuilder::CollectCameras(Game::World& world, std::vector<CameraData>& camerasOut)
	{
		RYU_PROFILE_SCOPE();

		auto& registry = world.GetRegistry();
		auto view = registry.view<Game::Transform, Game::Camera>();

		for (const auto& [entity, transform, camera] : view.each())
		{
			const CameraData camData = ExtractCameraData(transform, camera);
			camerasOut.push_back(camData);
		}

		// Sort by priority
		std::ranges::sort(camerasOut, [](const CameraData& a, const CameraData& b) { return a.Priority < b.Priority; });
	}

	CameraData RenderFrameBuilder::ExtractCameraData(const Game::Transform& transform, const Game::Camera& camera)
	{
		RYU_PROFILE_SCOPE();
		CameraData data{};

		// Position/orientation from entity Transform
		data.Position     = transform.Position;
		SM::Matrix rotMat = SM::Matrix::CreateFromQuaternion(transform.Rotation);
		data.Forward      = SM::Vector3(rotMat._31, rotMat._32, rotMat._33);
		SM::Vector3 up    = SM::Vector3(rotMat._21, rotMat._22, rotMat._23);

		// View matrix from transform
		data.ViewMatrix = SM::Matrix::CreateLookAt(
			transform.Position,
			transform.Position + data.Forward,
			up);

		// Projection from CameraComponent settings using stored screen size
		const f32 aspect = GetAspectRatio();

		if (camera.Mode == Game::Camera::ProjectionMode::Perspective)
		{
			data.ProjectionMatrix = SM::Matrix::CreatePerspectiveFieldOfView(
				DirectX::XMConvertToRadians(camera.FieldOfView),
				aspect,
				camera.NearPlane,
				camera.FarPlane);
		}
		else
		{
			data.ProjectionMatrix = SM::Matrix::CreateOrthographic(
				camera.OrthoSize * aspect * 2.0f,
				camera.OrthoSize * 2.0f,
				camera.NearPlane,
				camera.FarPlane);
		}

		data.ViewProjectionMatrix = data.ViewMatrix * data.ProjectionMatrix;
		data.NearPlane            = camera.NearPlane;
		data.FarPlane             = camera.FarPlane;
		data.CullingMask          = camera.CullingMask;
		data.Priority             = camera.Priority;

		// NOTE: Viewport is fullscreen (may be different for ImGui in the future?)
		data.ViewportX      = 0;
		data.ViewportY      = 0;
		data.ViewportWidth  = m_config.ScreenWidth;
		data.ViewportHeight = m_config.ScreenHeight;

		return data;
	}

	RenderItem RenderFrameBuilder::CreateRenderItem(const Game::Transform& transform, const Game::MeshRenderer& renderer)
	{
		return RenderItem
		{
			.MeshHandle     = renderer.MeshHandle,
			.WorldTransform = transform.GetWorldMatrix(),
			.RenderLayer    = renderer.RenderLayer
		};
	}

	u64 RenderFrameBuilder::ComputeSortKey(const RenderItem& item)
	{
		// Sort key encoding (64 bits):
		// [63-56] Render layer (8 bits)
		// [55-32] Material handle (24 bits) - for batching
		// [31-0]  Mesh handle (32 bits) - for batching

		u64 key = 0;
		key |= static_cast<u64>(item.RenderLayer) << 56;
		// Future: add material and mesh indices for batching
		return key;
	}
}
