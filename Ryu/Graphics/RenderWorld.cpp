#include "Graphics/RenderWorld.h"
#include "Graphics/Camera.h"
#include "Game/World/World.h"
#include "Game/Components/CameraComponent.h"
#include "Game/Components/TransformComponent.h"
#include "Game/Components/MeshRenderer.h"
#include <ranges>

namespace Ryu::Gfx
{
	RenderWorld::RenderWorld(Asset::AssetRegistry* registry)
		: m_assetRegistry(registry)
	{
	}

	RenderFrame RenderWorld::ExtractRenderData(Game::World& world, f32 deltaTime, f32 TotalTime, u64 frameNumber)
	{
		RenderFrame frame{};
		frame.DeltaTime  = deltaTime;
		frame.TotalTime  = TotalTime;
		frame.FrameNumber= frameNumber;

		// Collect all cameras, sorted by priority
		std::vector<CameraData> cameras;
		CollectCameras(world, cameras);

		// If no camera components exist, we use a default implementation
		// That's handled at a higher level (SceneRenderer)

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

    RenderView RenderWorld::ExtractViewForCamera(Game::World& world, const CameraData& camera)
    {
        RenderView view;
        view.CameraData = camera;

        CollectRenderables(
            world,
            camera.CullingMask,
            view.OpaqueItems,
            view.TransparentItems);

        return view;
    }

    void RenderWorld::CollectRenderables(Game::World& world, u32 cullingMask, std::vector<RenderItem>& opaqueOut, std::vector<RenderItem>& transparentOut)
    {
        UNREFERENCED_PARAMETER(transparentOut);

        auto& registry = world.GetRegistry();

        // Query all entities with Transform and MeshRenderer
        auto view = registry.view<Game::Transform, Game::MeshRenderer>();

        for (const auto& [entity, transform, renderer] :  view.each())
        {
            if (!renderer.IsVisible                                      // Skip invisible
                || ((cullingMask & (1u << renderer.RenderLayer)) == 0))  // Layer culling
            {
                continue;
            }

            RenderItem item = CreateRenderItem(transform, renderer);
            item.SortKey = ComputeSortKey(item);

            RYU_TODO("Check material for transparency")
            // For now, all items are opaque
            opaqueOut.push_back(item);
        }
    }

    void RenderWorld::CollectCameras(Game::World& world, std::vector<CameraData>& camerasOut)
    {
        auto& registry = world.GetRegistry();
        auto view = registry.view<Game::Transform, Game::Camera>();

        RYU_TODO("Remove these hard-coded numbers for testing and get actual screen size from somewhere")
        constexpr u32 screenWidth = 1280;
        constexpr u32 screenHeight = 720;

        for (const auto& [entity, transform, camera] : view.each())
        {
            const CameraData camData = ExtractCameraData(transform, camera, screenWidth, screenHeight);
            camerasOut.push_back(camData);
        }

        // Sort by priority
        std::sort(camerasOut.begin(), camerasOut.end(),
        [](const CameraData& a, const CameraData& b)
        {
            return a.Priority < b.Priority;
        });
    }

    CameraData RenderWorld::ExtractCameraData(const Game::Transform& transform, const Game::Camera& camera, u32 screenWidth, u32 screenHeight)
    {
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

        RYU_TODO("Better rework this logic to make use of the camera provided in Gfx or split the logic into a camera controller")
        // Projection from CameraComponent settings
        f32 aspect = (f32)screenWidth/ (f32)screenHeight;
        if (camera.Mode == Game::Camera::ProjectionMode::Perspective)
        {
            data.ProjectionMatrix = SM::Matrix::CreatePerspectiveFieldOfView(
                DirectX::XMConvertToRadians(camera.FieldOfView),
                aspect, camera.NearPlane, camera.FarPlane);
        }
        else
        {
            data.ProjectionMatrix = SM::Matrix::CreateOrthographic(
                camera.OrthoSize * aspect * 2.0f,
                camera.OrthoSize * 2.0f,
                camera.NearPlane, camera.FarPlane);
        }

        data.ViewProjectionMatrix = data.ViewMatrix * data.ProjectionMatrix;
        data.ViewportWidth        = screenWidth;
		data.ViewportHeight       = screenHeight;
        return data;
    }

    RenderItem RenderWorld::CreateRenderItem(const Game::Transform& transform, const Game::MeshRenderer& renderer)
    {
        return RenderItem
        {
            .MeshHandle     = renderer.MeshHandle,
            .WorldTransform = transform.GetWorldMatrix(),
            .RenderLayer    = renderer.RenderLayer
        };
    }

    u64 RenderWorld::ComputeSortKey(const RenderItem& item)
    {
        // Sort key encoding (64 bits):
        // [63-56] Render layer (8 bits)
        // [55-32] Material handle (24 bits) - for batching
        // [31-0]  Mesh handle (32 bits) - for batching

        u64 key = 0;
        key |= static_cast<u64>(item.RenderLayer) << 56;
        // Future material and mesh logic
        //key |= static_cast<u64>(item.Material.Index & 0xFFFFFF) << 32;
        //key |= static_cast<u64>(item.Mesh.Index);
        //
        return key;
    }
}
