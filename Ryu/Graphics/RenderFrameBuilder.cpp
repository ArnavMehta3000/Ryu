#include "Graphics/RenderFrameBuilder.h"

#include "Core/Profiling/Profiling.h"
#include "Game/Components/CameraComponent.h"
#include "Game/Components/MeshRenderer.h"
#include "Game/Components/TransformComponent.h"
#include "Game/World/World.h"
#include <ranges>

namespace Ryu::Gfx
{
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

	RenderView RenderFrameBuilder::ExtractViewForCamera(Game::World& world, const CameraData& cameraData)
	{
		RYU_PROFILE_SCOPE();

		RenderView view { .CameraData = cameraData };

		CollectRenderables(
			world,
			cameraData.CullingMask,
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
		auto view = registry.view<Game::Transform, Game::CameraComponent>();

		for (const auto& [entity, transform, camera] : view.each())
		{
			// Check if the camera viewport is valid
			bool isViewportValid = camera.Viewport.width > 0 && camera.Viewport.height > 0;
			RYU_ASSERT(isViewportValid, "Invalid camera viewport");

			const CameraData camData = ExtractCameraData(transform, camera);
			camerasOut.push_back(camData);
		}

		// Sort by priority
		std::ranges::sort(camerasOut, [](const CameraData& a, const CameraData& b) { return a.Priority < b.Priority; });
	}

	CameraData RenderFrameBuilder::ExtractCameraData(const Game::Transform& transform, const Game::CameraComponent& camera)
	{
		RYU_PROFILE_SCOPE();
		CameraData data
		{
			.Viewport    = camera.Viewport,
			.Position    = transform.Position,
			.NearPlane   = camera.ClipPlane[0],
			.FarPlane    = camera.ClipPlane[1],
			.CullingMask = camera.CullingMask,
			.Priority    = camera.Priority
		};

		// Position/orientation from entity Transform
		SM::Matrix rotMat = SM::Matrix::CreateFromQuaternion(transform.Orientation);
		data.Forward      = SM::Vector3(rotMat._31, rotMat._32, rotMat._33);
		SM::Vector3 up    = SM::Vector3(rotMat._21, rotMat._22, rotMat._23);

		// View matrix from transform
		data.ViewMatrix = SM::Matrix::CreateLookAt(
			transform.Position,
			transform.Position + data.Forward,
			up);

		if (camera.Mode == Game::CameraComponent::Projection::Perspective)
		{
			data.ProjectionMatrix = SM::Matrix::CreatePerspectiveFieldOfView(
				DirectX::XMConvertToRadians(camera.FovY),
				camera.Viewport.AspectRatio(),
				data.NearPlane, data.FarPlane);
		}
		else
		{
			data.ProjectionMatrix = SM::Matrix::CreateOrthographic(
				camera.OrthoSize[0], camera.OrthoSize[1],
				data.NearPlane, data.FarPlane);
		}

		data.ViewProjectionMatrix = data.ViewMatrix * data.ProjectionMatrix;

		// NOTE: Viewport is fullscreen (may be different for ImGui in the future?)

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
