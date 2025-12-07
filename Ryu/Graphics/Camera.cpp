#include "Graphics/Camera.h"
#include <numbers>

namespace Ryu::Gfx
{
	using namespace Math;
	
	Camera::Camera()
        : m_position(0.0f, 0.0f, -5.0f)
        , m_forward(0.0f, 0.0f, 1.0f)
        , m_right(1.0f, 0.0f, 0.0f)
        , m_up(0.0f, 1.0f, 0.0f)
        , m_viewportWidth(1920.0f)
        , m_viewportHeight(1080.0f)
	{
        UpdateVectors();
        UpdateViewMatrix();
        UpdateProjectionMatrix();
	}

    void Camera::Update(f32 deltaTime)
    {
        if (m_isInterpolating)
        {
            m_interpolationProgress += deltaTime * m_interpolationSpeed;

            if (m_interpolationProgress >= 1.0f)
            {
                m_interpolationProgress = 1.0f;
                m_isInterpolating       = false;
                m_currentMode           = m_targetMode;
            }
        }
    }

    void Camera::SetViewportSize(f32 width, f32 height)
    {
        m_viewportWidth = width;
        m_viewportHeight = height;
        m_aspectRatio = width / height;
        UpdateProjectionMatrix();
    }

    void Camera::SetPosition(const Vector3& position)
    {
        m_position = position;
        UpdateViewMatrix();
    }

    void Camera::SetOrientation(const Quaternion& orientation)
    {
        m_orientation = orientation;
        m_orientation.Normalize();

        UpdateVectors();
        UpdateViewMatrix();
    }

    void Camera::Move(const Vector3& direction, f32 speed)
    {
        m_position += direction * speed;
        UpdateViewMatrix();
    }

    void Camera::Rotate(f32 deltaPitch, f32 deltaYaw)
    {
        // Rotate around local right axis (pitch)
        Quaternion pitchRotation = Quaternion::CreateFromAxisAngle(m_right, deltaPitch);

        // Rotate around world up axis (yaw)
        Quaternion yawRotation = Quaternion::CreateFromAxisAngle(Vector3::Up, deltaYaw);

        // Combine rotations: yaw first, then pitch
        m_orientation = pitchRotation * m_orientation * yawRotation;
        m_orientation.Normalize();

        UpdateVectors();
        UpdateViewMatrix();
    }

    void Camera::RotateAroundAxis(const Vector3& axis, f32 angle)
    {
        Quaternion rotation = Quaternion::CreateFromAxisAngle(axis, angle);
        m_orientation = rotation * m_orientation;
        m_orientation.Normalize();

        UpdateVectors();
        UpdateViewMatrix();
    }

    void Camera::Zoom(f32 delta)
    {
        if (m_targetMode == ProjectionMode::Perspective || m_currentMode == ProjectionMode::Perspective)
        {
            m_fov = std::max(10.0f, std::min(120.0f, m_fov - delta));
        }
        else
        {
            m_orthoSize = std::max(0.1f, m_orthoSize - delta * 0.1f);
        }

        UpdateProjectionMatrix();
    }

    void Camera::SetFieldOfView(f32 fovDegrees)
    {
        m_fov = std::max(10.0f, std::min(120.0f, fovDegrees));
        UpdateProjectionMatrix();
    }

    void Camera::SetOrthographicSize(f32 size)
    {
        m_orthoSize = std::max(0.1f, size);
        UpdateProjectionMatrix();
    }

    void Camera::SetProjectionMode(ProjectionMode mode, bool interpolate)
    {
        if (m_targetMode == mode && !m_isInterpolating)
        {
            return;
        }

        m_targetMode = mode;

        if (interpolate)
        {
            m_isInterpolating       = true;
            m_interpolationProgress = 0.0f;
        }
        else
        {
            m_currentMode           = mode;
            m_isInterpolating       = false;
            m_interpolationProgress = 0.0f;
        }
    }

    Matrix Camera::GetProjectionMatrix() const
    {
        if (m_isInterpolating)
        {
            const Matrix& from = (m_currentMode == ProjectionMode::Perspective) ? m_perspectiveMatrix : m_orthographicMatrix;
            const Matrix& to = (m_targetMode == ProjectionMode::Perspective) ? m_perspectiveMatrix : m_orthographicMatrix;

            return LerpProjectionMatrix(from, to, m_interpolationProgress);
        }

        return (m_currentMode == ProjectionMode::Perspective) ? m_perspectiveMatrix : m_orthographicMatrix;
    }

    Math::Matrix Camera::GetViewProjectionMatrix() const
    {
        return m_viewMatrix * GetProjectionMatrix();
    }

    void Camera::UpdateViewMatrix()
    {
        Vector3 target = m_position + m_forward;
        m_viewMatrix = Matrix::CreateLookAt(m_position, target, m_up);
    }

    void Camera::UpdateProjectionMatrix()
    {
        // Perspective projection
        f32 fovRadians = DirectX::XMConvertToRadians(m_fov);
        m_perspectiveMatrix = Matrix::CreatePerspectiveFieldOfView(fovRadians, m_aspectRatio, m_nearPlane, m_farPlane);

        // Orthographic projection
        f32 orthoWidth = m_orthoSize * m_aspectRatio;
        f32 orthoHeight = m_orthoSize;
        m_orthographicMatrix = Matrix::CreateOrthographic(orthoWidth * 2.0f, orthoHeight * 2.0f, m_nearPlane, m_farPlane);
    }

    void Camera::UpdateVectors()
    {
        Matrix rotationMatrix = Matrix::CreateFromQuaternion(m_orientation);

        m_right   = Vector3(rotationMatrix._11, rotationMatrix._12, rotationMatrix._13);
        m_up      = Vector3(rotationMatrix._21, rotationMatrix._22, rotationMatrix._23);
        m_forward = Vector3(rotationMatrix._31, rotationMatrix._32, rotationMatrix._33);
    }

    Matrix Camera::LerpProjectionMatrix(const Matrix& from, const Matrix& to, f32 t) const
    {
        t = t * t * (3.0f - 2.0f * t);  // Using smoothstep
        return Matrix::Lerp(from, to, t);
    }
}
