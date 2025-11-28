#include "Game/Components/Transform.h"

namespace Ryu::Game
{
	Transform::Transform(const SM::Vector3& position, const SM::Quaternion& rotation, const SM::Vector3& scale)
		: Position(position), Rotation(rotation), Scale(scale)
	{
	}

	Transform::Transform(const SM::Vector3& position, const SM::Quaternion& rotation)
		: Transform(position, rotation, SM::Vector3::One)
	{
	}

	Transform::Transform(const SM::Vector3& position)
		: Transform(position, SM::Quaternion::Identity, SM::Vector3::One)
	{
	}

	Transform::Transform()
		: Transform(SM::Vector3::Zero, SM::Quaternion::Identity, SM::Vector3::One)
	{
	}
}
