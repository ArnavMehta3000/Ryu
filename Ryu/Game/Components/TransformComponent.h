#pragma once
#include "Math/Math.h"
#include "Core/Utils/Reflection.h"

namespace Ryu::Game
{
	struct Transform
	{
		RYU_ENABLE_REFLECTION(Transform)

	public:
		Transform(const SM::Vector3& position, const SM::Quaternion& rotation, const SM::Vector3& scale);
		Transform(const SM::Vector3& position);
		Transform(const SM::Vector3& position, const SM::Quaternion& rotation);
		Transform();

		[[nodiscard]] static inline SM::Matrix ComputeWorldMatrix(const Transform& t)
		{
			return SM::Matrix::CreateScale(t.Scale)
				* SM::Matrix::CreateFromQuaternion(t.Rotation)
				* SM::Matrix::CreateTranslation(t.Position);
		}

		[[nodiscard]] inline Math::Matrix GetWorldMatrix() const { return ComputeWorldMatrix(*this); }

		SM::Vector3 Position;
		SM::Quaternion Rotation;
		SM::Vector3 Scale;
	};
}

RYU_REFLECTED_CLASS(
	::Ryu::Game::Transform,
	"Transform",
	RYU_CLASS_ATTRIB(::Ryu::Game::Transform, Position),
	RYU_CLASS_ATTRIB(::Ryu::Game::Transform, Rotation),
	RYU_CLASS_ATTRIB(::Ryu::Game::Transform, Scale)
)
