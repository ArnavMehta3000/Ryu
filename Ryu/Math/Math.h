#pragma once
#include <Windows.h>
#include <External/SimpleMath/SimpleMath.h>
#include "Utils/Serializer.h"

namespace SM = DirectX::SimpleMath;

namespace Ryu::Math
{
	using namespace DirectX::SimpleMath;

	constexpr auto BytesToKB(auto bytes) { return bytes / 1024.0f; }
	constexpr auto BytesToMB(auto bytes) { return BytesToKB(bytes) / 1024.0f; }
	constexpr auto BytesToGB(auto bytes) { return BytesToMB(bytes) / 1024.0f; }
}


namespace Ryu::Utils
{
	template<>
	struct Serializer<DirectX::SimpleMath::Vector2>
	{
		static void Serialize(const DirectX::SimpleMath::Vector2& vec, toml::array& arr)
		{
			arr.push_back(vec.x);
			arr.push_back(vec.y);
		}
	};

	template<>
	struct Deserializer<DirectX::SimpleMath::Vector2>
	{
		static void Deserialize(DirectX::SimpleMath::Vector2& vec, const toml::array& arr)
		{
			if (arr.size() >= 1) vec.x = arr[0].value_or(0.0f);
			if (arr.size() >= 2) vec.y = arr[1].value_or(0.0f);
		}
	};

	// --------------------------------------------------------------------------------------

	template<>
	struct Serializer<DirectX::SimpleMath::Vector3>
	{
		static void Serialize(const DirectX::SimpleMath::Vector3& vec, toml::array& arr)
		{
			arr.push_back(vec.x);
			arr.push_back(vec.y);
			arr.push_back(vec.z);
		}
	};

	template<>
	struct Deserializer<DirectX::SimpleMath::Vector3>
	{
		static void Deserialize(DirectX::SimpleMath::Vector3& vec, const toml::array& arr)
		{
			if (arr.size() >= 1) vec.x = arr[0].value_or(0.0f);
			if (arr.size() >= 2) vec.y = arr[1].value_or(0.0f);
			if (arr.size() >= 3) vec.z = arr[2].value_or(0.0f);
		}
	};

	// --------------------------------------------------------------------------------------

	template<>
	struct Serializer<DirectX::SimpleMath::Vector4>
	{
		static void Serialize(const DirectX::SimpleMath::Vector4& vec, toml::array& arr)
		{
			arr.push_back(vec.x);
			arr.push_back(vec.y);
			arr.push_back(vec.z);
			arr.push_back(vec.w);
		}
	};

	template<>
	struct Deserializer<DirectX::SimpleMath::Vector4>
	{
		static void Deserialize(DirectX::SimpleMath::Vector4& vec, const toml::array& arr)
		{
			if (arr.size() >= 1) vec.x = arr[0].value_or(0.0f);
			if (arr.size() >= 2) vec.y = arr[1].value_or(0.0f);
			if (arr.size() >= 3) vec.z = arr[2].value_or(0.0f);
			if (arr.size() >= 4) vec.w = arr[3].value_or(0.0f);
		}
	};

	// --------------------------------------------------------------------------------------

	template<>
	struct Serializer<DirectX::SimpleMath::Quaternion>
	{
		static void Serialize(const DirectX::SimpleMath::Quaternion& q, toml::array& arr)
		{
			arr.push_back(q.x);
			arr.push_back(q.y);
			arr.push_back(q.z);
			arr.push_back(q.w);
		}
	};

	template<>
	struct Deserializer<DirectX::SimpleMath::Quaternion>
	{
		static void Deserialize(DirectX::SimpleMath::Quaternion& q, const toml::array& arr)
		{
			if (arr.size() >= 1) q.x = arr[0].value_or(0.0f);
			if (arr.size() >= 2) q.y = arr[1].value_or(0.0f);
			if (arr.size() >= 3) q.z = arr[2].value_or(0.0f);
			if (arr.size() >= 4) q.w = arr[3].value_or(1.0f);
		}
	};
}
