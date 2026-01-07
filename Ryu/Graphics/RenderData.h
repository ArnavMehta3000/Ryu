#pragma once
#include "Asset/AssetData.h"
#include "Math/Math.h"

namespace Ryu::Gfx
{
	// Data for a single renderable item
	struct RenderItem
	{
		Asset::MeshHandle MeshHandle;
		Math::Matrix WorldTransform;
		u8 RenderLayer = 0;
		u64 SortKey = 0;  // Sorting key for batching
	};

	// Camera data extracted from the camera component + transform
	struct CameraData
	{
		// Maybe cache these in the component itself?
		Math::Viewport Viewport;  // Using the depth values as the clip plane
		Math::Matrix ViewMatrix;
		Math::Matrix ProjectionMatrix;
		Math::Matrix ViewProjectionMatrix;
		Math::Vector3 Position;
		Math::Vector3 Forward;
		f32 NearPlane = 0.1f;
		f32 FarPlane  = 1000.0f;
		u32 CullingMask = 0xFFFFFFFF;
		u32 Priority    = 0;
	};

	// Collection of render items for a single camera view
	struct RenderView
	{
		CameraData CameraData;
		std::vector<RenderItem> OpaqueItems;
		std::vector<RenderItem> TransparentItems;
		// Add lights, shadow casters etc when needed
	};

	// Complete render frame data
	struct RenderFrame
	{
		std::vector<RenderView> Views;
		f32 DeltaTime;
		f32 TotalTime;
		u64 FrameNumber;
	};
}
