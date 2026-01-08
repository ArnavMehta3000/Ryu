#include "Graphics/RenderFrameBuilder.h"

#include "Core/Profiling/Profiling.h"
#include "Game/Components/CameraComponent.h"
#include "Game/Components/MeshRenderer.h"
#include "Game/Components/TransformComponent.h"
#include "Game/World/World.h"
#include "Graphics/Core/GfxDevice.h"
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
			const CameraData camData = ExtractCameraData(transform, camera);
			camerasOut.push_back(camData);
		}

		// Sort by priority
		std::ranges::sort(camerasOut, [](const CameraData& a, const CameraData& b) { return a.Priority < b.Priority; });
	}

	CameraData RenderFrameBuilder::ExtractCameraData(const Game::Transform& transform, const Game::CameraComponent& camera)
	{
		RYU_PROFILE_SCOPE();

		// NOTE: Using the window as the viewport size
		// Maybe instead pass in the frame dimensions (consideration for ImGui)
		// Or somehow embedd that information into the camera component
		auto [width, height] = m_device->GetClientSize();

		// Temporary clamp the size (otherwise we assert inside DirectXMath when creating the projection matrix)
		// Assertion Assertion failed: !XMScalarNearEqual(AspectRatio, 0.0f, 0.00001f)
		width  = std::max(width, 10u);
		height = std::max(height, 10u);

		CameraData data
		{
			.Position    = transform.Position,
			.NearPlane   = camera.ClipPlane[0],
			.FarPlane    = camera.ClipPlane[1],
			.CullingMask = camera.CullingMask,
			.Priority    = camera.Priority
		};

		data.Viewport.x = data.Viewport.y = 0.0f;
		data.Viewport.width  = static_cast<f32>(width);
		data.Viewport.height = static_cast<f32>(height);

		// Position/orientation from entity Transform
		SM::Matrix rotMat = SM::Matrix::CreateFromQuaternion(transform.Orientation);
		data.Forward      = SM::Vector3::Transform(SM::Vector3::Forward, transform.Orientation);
		SM::Vector3 up    = SM::Vector3::Transform(SM::Vector3::Up, transform.Orientation);

		// Create a view matrix from transform
		data.ViewMatrix = SM::Matrix::CreateLookAt(//DirectX::XMMatrixLookAtLH(
			transform.Position,
			transform.Position + data.Forward,
			up);

		const f32 aspect = data.Viewport.AspectRatio();

		// Build camera matrices
		if (camera.Mode == Game::CameraComponent::Projection::Perspective)
		{
			f32 fov = DirectX::XMConvertToRadians(camera.FovY);
			if (DirectX::XMScalarNearEqual(fov, 0.0f, 0.00001f * 2.0f))
			{
				// Slightly bigger than the above check
				constexpr f32 updatedFov = 0.000015f * 2.0f;
				fov = updatedFov;
			}
			
			data.ProjectionMatrix = SM::Matrix::CreatePerspectiveFieldOfView(//DirectX::XMMatrixPerspectiveFovLH(
				fov, aspect, data.NearPlane, data.FarPlane);
		}
		else
		{
			// OrthoSize[1] is the vertical size; compute horizontal from aspect
			f32 orthoHeight = camera.OrthoSize[1];
			f32 orthoWidth = orthoHeight * aspect;

			data.ProjectionMatrix = SM::Matrix::CreateOrthographic(//DirectX::XMMatrixOrthographicLH(
				orthoWidth, orthoHeight,
				data.NearPlane, data.FarPlane);
		}

		data.ViewProjectionMatrix = data.ViewMatrix * data.ProjectionMatrix;
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
