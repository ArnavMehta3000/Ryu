#pragma once
#include "Common/StandardTypes.h"
#include "Math/Math.h"
#include <numbers>

namespace Ryu::Gfx
{
	class CameraBase
	{
	public:
		CameraBase() = default;
		~CameraBase() = default;

		[[nodiscard]] inline f32 GetFOV() const noexcept { return m_fovY; }
		[[nodiscard]] inline f32 GetNearZ() const noexcept { return m_nearZ; }
		[[nodiscard]] inline f32 GetFarZ() const noexcept { return m_farZ; }
		[[nodiscard]] inline f32 GetAspectRatio() const noexcept { return m_viewportWidth / m_viewportHeight; }
		[[nodiscard]] inline f32 GetViewportWidth() const noexcept { return m_viewportWidth; }
		[[nodiscard]] inline f32 GetViewportHeight() const noexcept { return m_viewportHeight; }
		[[nodiscard]] inline const Math::Vector3& GetPosition() const noexcept { return m_position; }
		[[nodiscard]] inline const Math::Quaternion& GetRotation() const noexcept { return m_rotation; }
		[[nodiscard]] inline Math::Matrix GetViewProjectionMatrix() const noexcept { return m_viewMat * m_projectionMat; }
		
		[[nodiscard]] const Math::Matrix& GetViewMatrix() const noexcept;
		[[nodiscard]] const Math::Matrix& GetProjectionMatrix() const noexcept;
		
		void SetViewport(f32 w, f32 h);

		void SetLens(f32 fovY, f32 w, f32 h, f32 nearZ, f32 farZ);

	protected:
		virtual void UpdateProjectionMatrix() const = 0;
		virtual void UpdateViewMatrix() const;

	protected:
		Math::Vector3        m_position;
		Math::Quaternion     m_rotation;

		f32                  m_fovY            = 0.0f;
		f32                  m_nearZ           = 0.1f;
		f32                  m_farZ            = 1000.0f;
		f32                  m_viewportWidth   = 1920.0f;
		f32                  m_viewportHeight  = 1080.0f;

		mutable Math::Matrix m_viewMat;
		mutable Math::Matrix m_projectionMat;
		mutable bool         m_viewDirty       = true;
		mutable bool         m_projectionDirty = true;
	};


	class PerspectiveCamera : public CameraBase
	{
	protected:
		void UpdateProjectionMatrix() const override;
	};

	class OrthoGraphicCamera : public CameraBase
	{
	protected:
		void UpdateProjectionMatrix() const override;
	};
}
