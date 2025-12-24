#pragma once
#include "Core/Utils/Reflection.h"
#include "Core/Utils/Serializer.h"
#include <concepts>

namespace Ryu::Utils
{
	// Helper to check if a type is a native TOML value type
	template<typename T>
	concept TomlInsertable =
		std::same_as<T, std::string>
		|| std::same_as<T, int64_t>
		|| std::same_as<T, double>
		|| std::same_as<T, bool>
		|| std::same_as<T, toml::date>
		|| std::same_as<T, toml::time>
		|| std::same_as<T, toml::date_time>
		|| std::integral<T>
		|| std::floating_point<T>;

	// Check if type serializes to array
	template<typename T>
	concept SerializesToArray = requires(const T& t, toml::array& arr)
	{
		Serializer<T>::Serialize(t, arr);
	};

	// Check if type deserializes from array
	template<typename T>
	concept DeserializesFromArray = requires(T& t, const toml::array& arr)
	{
		Deserializer<T>::Deserialize(t, arr);
	};

	// Automatic serializer for reflected types
	template <Reflection::IsReflected T>
	struct Serializer<T>
	{
		static void Serialize(const T& obj, toml::table& table)
		{
			Reflection::ForEachAttribute<T>([&](const auto& attr)
			{
				using AttrType = typename std::remove_cvref_t<decltype(attr)>::Type;
				const auto& value = attr.GetValue(obj);

				if constexpr (TomlInsertable<AttrType>)
				{
					// Direct insertion for primitives
					table.insert(std::string(attr.GetName()), value);
				}
				else if constexpr (SerializesToArray<AttrType>)
				{
					// Serialize to array
					toml::array arr;
					Serializer<AttrType>::Serialize(value, arr);
					table.insert(std::string(attr.GetName()), arr);
				}
				else if constexpr (Reflection::IsReflected<AttrType>)
				{
					// Nested reflected type
					toml::table nested;
					Serializer<AttrType>::Serialize(value, nested);
					table.insert(std::string(attr.GetName()), nested);
				}
				else if constexpr (Serializable<AttrType>)
				{
					// Custom serializer exists
					toml::table nested;
					Serializer<AttrType>::Serialize(value, nested);
					table.insert(std::string(attr.GetName()), nested);
				}
				else
				{
					RYU_ASSERT(false, "Trying to serialize unsupported attribute type");
				}
			});
		}
	};

	// Automatic deserializer for reflected types
	template <Reflection::IsReflected T>
	struct Deserializer<T>
	{
		static void Deserialize(T& obj, const toml::table& table)
		{
			Reflection::ForEachAttribute<T>([&](const auto& attr)
			{
				using AttrType = typename std::remove_cvref_t<decltype(attr)>::Type;
				auto fieldName = std::string(attr.GetName());

				if (auto node = table.get(fieldName))
				{
					if constexpr (TomlInsertable<AttrType>)
					{
						// Direct extraction for primitives
						if (auto val = node->template value<AttrType>())
						{
							attr.SetValue(obj, *val);
						}
					}
					else if constexpr (DeserializesFromArray<AttrType>)
					{
						// Deserialize from array
						if (auto arr = node->as_array())
						{
							AttrType value;
							Deserializer<AttrType>::Deserialize(value, *arr);
							attr.SetValue(obj, std::move(value));
						}
					}
					else if constexpr (Reflection::IsReflected<AttrType>)
					{
						// Nested reflected type
						if (auto nested = node->as_table())
						{
							AttrType value;
							Deserializer<AttrType>::Deserialize(value, *nested);
							attr.SetValue(obj, std::move(value));
						}
					}
					else if constexpr (Deserializable<AttrType>)
					{
						// Custom deserializer exists
						if (auto nested = node->as_table())
						{
							AttrType value;
							Deserializer<AttrType>::Deserialize(value, *nested);
							attr.SetValue(obj, std::move(value));
						}
					}
					else
					{
						RYU_ASSERT(false, "Trying to deserialize unsupported attribute type");
					}
				}
			});
		}
	};
}
