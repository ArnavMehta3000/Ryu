#pragma once
#include "Math/Math.h"
#include <numbers>

namespace Ryu::Gfx
{
	class Camera
	{
	public:
		enum class ProjectionMode { Perspective, Orthographic };

	public:
		Camera();
		~Camera() = default;

		void Update(f32 dt);
		void SetViewportSize(f32 width, f32 height);

		void SetPosition(const Math::Vector3& position);
		void SetOrientation(const Math::Quaternion& orientation);
		void Move(const Math::Vector3& direction, f32 speed);
		void Rotate(f32 deltaPitch, f32 deltaYaw);
		void RotateAroundAxis(const Math::Vector3& axis, f32 angle);

		void Zoom(f32 delta);
		void SetFieldOfView(f32 fovDegrees);
		void SetOrthographicSize(f32 size);

		void SetProjectionMode(ProjectionMode mode, bool interpolate = false);
		void SetInterpolationSpeed(f32 speed) { m_interpolationSpeed = speed; }
		void SetNearPlane(f32 nearPlane) { m_nearPlane = nearPlane; UpdateProjectionMatrix(); }
		void SetFarPlane(f32 farPlane) { m_farPlane = farPlane; UpdateProjectionMatrix(); }

		[[nodiscard]] Math::Matrix GetViewMatrix() const { return m_viewMatrix; }
		[[nodiscard]] Math::Matrix GetProjectionMatrix() const;
		[[nodiscard]] Math::Matrix GetViewProjectionMatrix() const;

		[[nodiscard]] inline Math::Vector3 GetPosition() const { return m_position; }
		[[nodiscard]] inline Math::Quaternion GetOrientation() const { return m_orientation; }
		[[nodiscard]] inline Math::Vector3 GetForward() const { return m_forward; }
		[[nodiscard]] inline Math::Vector3 GetRight() const { return m_right; }
		[[nodiscard]] inline Math::Vector3 GetUp() const { return m_up; }
		[[nodiscard]] inline f32 GetFieldOfView() const { return m_fov; }
		[[nodiscard]] inline f32 GetOrthographicSize() const { return m_orthoSize; }
		[[nodiscard]] inline ProjectionMode GetProjectionMode() const { return m_targetMode; }
		[[nodiscard]] inline f32 GetNearPlane() const { return m_nearPlane; }
		[[nodiscard]] inline f32 GetFarPlane() const { return m_farPlane; }

	private:
		void UpdateViewMatrix();
		void UpdateProjectionMatrix();
		void UpdateVectors();
		Math::Matrix LerpProjectionMatrix(const Math::Matrix& from, const Math::Matrix& to, f32 t) const;

	private:
		Math::Vector3    m_position;
		Math::Quaternion m_orientation;
		Math::Vector3    m_forward;
		Math::Vector3    m_right;
		Math::Vector3    m_up;

		f32              m_fov                   = 60.0f;        // In degrees
		f32              m_orthoSize             = 5.0f;         // Half height of ortho projection
		f32              m_aspectRatio           = 16.0f / 9.0f;
		f32              m_nearPlane             = 0.1f;
		f32              m_farPlane              = 1000.0f;
		f32              m_viewportWidth;
		f32              m_viewportHeight;

		ProjectionMode   m_currentMode           = ProjectionMode::Perspective;
		ProjectionMode   m_targetMode            = ProjectionMode::Perspective;
		bool             m_isInterpolating       = false;
		f32              m_interpolationProgress = 0.0f;
		f32              m_interpolationSpeed    = 2.0f;

		Math::Matrix     m_viewMatrix;
		Math::Matrix     m_perspectiveMatrix;
		Math::Matrix     m_orthographicMatrix;
	};
}
