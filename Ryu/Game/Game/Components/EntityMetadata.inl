#include "Utils/Serializer.h"

namespace Ryu::Utils
{
	// Serializer for EntityFlag
	template<>
	struct Serializer<Game::EntityFlag> 
	{
		static void Serialize(const Game::EntityFlag& flag, toml::table& table) 
		{
			table.insert("value", static_cast<u32>(flag));
		}
	};

	// Deserializer for EntityFlag
	template<>
	struct Deserializer<Game::EntityFlag> 
	{
		static Game::EntityFlag Deserialize(const toml::table& table) 
		{
			return static_cast<Game::EntityFlag>(table["value"].value_or(0u));
		}
	};

	// Serialization for EntityFlags bitset
	template<>
	struct Serializer<Game::EntityFlags>
	{
		static void Serialize(const Game::EntityFlags& flags, toml::table& table)
		{
			table.insert("flags", flags.to_ulong());
		}
	};

	// Deserializer for EntityFlags
	template<>
	struct Deserializer<Game::EntityFlags>
	{
		static Game::EntityFlags Deserialize(const toml::table& table)
		{
			return Game::EntityFlags(table["flags"].value_or<unsigned long>(0u));
		}
	};

	// Serialization for EntityMetadata
	template<>
	struct Serializer<Game::EntityMetadata> 
	{
		static void Serialize(const Game::EntityMetadata& metadata, toml::table& table)
		{
			table.insert("name", metadata.Name);
			table.insert("uuid", metadata.GetUUIDPretty());
			table.insert("flags", metadata.Flags.to_ulong());
		}
	};

	// Deserializer for EntityMetadata
	template<>
	struct Deserializer<Game::EntityMetadata>
	{
		static Game::EntityMetadata Deserialize(const toml::table& table)
		{
			Game::EntityMetadata metadata;
			Deserialize(metadata, table);
			return metadata;
		}
		
		static void Deserialize(Game::EntityMetadata& metadata, const toml::table& table)
		{
			// Either provide the cached uuid or generate a new one
			metadata = Game::EntityMetadata(table["uuid"].value_or(std::string{}));

			metadata.Name = table["name"].value_or(std::string{});
			metadata.Flags = Game::EntityFlags(table["flags"].value_or<unsigned long>(0u));
		}
	};
}
