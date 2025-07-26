#pragma once
#include "Math/Math.h"

namespace Ryu::Game
{
	class Transform
	{
	public:
		Transform(const SM::Vector3& position, const SM::Quaternion& rotation, const SM::Vector3& scale);
		Transform(const SM::Vector3& position);
		Transform(const SM::Vector3& position, const SM::Quaternion& rotation);
		Transform();

		SM::Vector3 Position;
		SM::Quaternion Rotation;
		SM::Vector3 Scale;
	};
}

#include "Game/Components/Transform.inl"
