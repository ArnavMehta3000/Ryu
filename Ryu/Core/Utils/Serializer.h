#pragma once
#include "Core/Common/Concepts.h"
#include <toml++/toml.hpp>

namespace Ryu::Utils
{
	template <typename T>
	struct Serializer;

	template <typename T>
	struct Deserializer;

	template <typename T>
	concept Serializable = requires (const T& t, toml::table& table)
	{
		Serializer<T>::Serialize(t, table);
	};

	template<typename T>
	concept Deserializable = requires(const toml::table& table)
	{
		// Take in a table and return a type
		{ Deserializer<T>::Deserialize(table) } -> std::convertible_to<T>;
	} || requires(T& t, const toml::table& table)
	{
		// Take in a type (to dump into) and a table
		Deserializer<T>::Deserialize(t, table);
	};

	// Generic functions that use the specializations
	template<Serializable T>
	void Serialize(const T& obj, toml::table& table)
	{
		Serializer<T>::Serialize(obj, table);
	}

	// Create the object
	template<Deserializable T>
	T Deserialize(const toml::table& table)
	{
		return Deserializer<T>::Deserialize(table);
	}
	
	// Dump the object details
	template<Deserializable T>
	void Deserialize(T& obj, const toml::table& table)
	{
		Deserializer<T>::Deserialize(obj, table);
	}
}
