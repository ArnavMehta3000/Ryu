#pragma once

namespace Ryu::Game
{
	struct CameraComponent
	{
		enum class Projection { Perspective, Orthographic };

		Projection Mode               = Projection::Perspective;
		
		f32 FovY                      = 60.0f;
		
		std::array<f32, 2> OrthoSize  = { 5.0f, 5.0f };     // Width, height
		std::array<f32, 2> ClipPlane  = { 0.1f, 1000.0f };  // Near, far
		std::array<f32, 4> ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		
		u32 CullingMask               = 0xFFFFFFFF;
		u32 Priority                  = 0;
		
		bool ShouldClear              = true;
		bool IsEnabled                = true;

		Math::Viewport Viewport;
	};
}

