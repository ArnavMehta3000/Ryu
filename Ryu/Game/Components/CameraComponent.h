#pragma once
#include "Graphics/Camera.h"

namespace Ryu::Game
{
	struct Camera
	{
		using ProjectionMode = Gfx::Camera::ProjectionMode;

		f32 FieldOfView = 60.0f;
		f32 NearPlane   = 0.1f;
		f32 FarPlane    = 1000.0f;
		ProjectionMode Mode = ProjectionMode::Perspective;
		f32 OrthoSize   = 5.0f;
		i32 Priority    = 0;
		u32 CullingMask = 0xFFFFFFFF;
	};
}
