#include "Graphics/Camera.h"

namespace Ryu::Gfx
{
	using namespace Math;

	const Math::Matrix& CameraBase::GetViewMatrix() const noexcept
	{
		if (m_viewDirty)
		{
			UpdateViewMatrix();
		}

		return m_viewMat;
	}
	const Math::Matrix& CameraBase::GetProjectionMatrix() const noexcept
	{
		if (m_projectionDirty)
		{
			UpdateProjectionMatrix();
		}

		return m_projectionMat;
	}
	
	void CameraBase::SetViewport(f32 w, f32 h)
	{
		m_viewportWidth   = w;
		m_viewportHeight  = h;
		m_projectionDirty = true;
	}
	
	void CameraBase::SetLens(f32 fovY, f32 w, f32 h, f32 nearZ, f32 farZ)
	{
		m_fovY           = fovY;
		m_nearZ          = nearZ;
		m_farZ           = farZ;
		m_viewportWidth  = w;
		m_viewportHeight = h;

		m_projectionDirty = true;
	}
	
	void CameraBase::UpdateViewMatrix() const
	{
		// TODO: Update view matrix
		
		m_viewDirty = false;
	}
	
	void PerspectiveCamera::UpdateProjectionMatrix() const
	{
		m_projectionMat = Matrix::CreatePerspective(m_viewportHeight, m_viewportWidth, m_nearZ, m_farZ);
		m_projectionDirty = false;
	}
	void OrthoGraphicCamera::UpdateProjectionMatrix() const
	{
		m_projectionMat = Matrix::CreateOrthographic(m_viewportWidth, m_viewportHeight, m_nearZ, m_farZ);
		m_projectionDirty = false;
	}
}
