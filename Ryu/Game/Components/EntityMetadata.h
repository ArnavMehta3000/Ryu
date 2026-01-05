#pragma once
#include "Core/Utils/UUID.h"
#include "Core/Utils/Reflection.h"
#include <bitset>

namespace Ryu::Game
{
	enum class EntityFlag : u32
	{
		None             = 0,
		MarkedForDestroy = RYU_ENUM_BIT(0),
		Disabled         = RYU_ENUM_BIT(1),

		MAX_FLAGS = 32
	};
	RYU_ENUM_ENABLE_BITMASK_OPERATORS(EntityFlag)

	using EntityFlags = std::bitset<static_cast<u64>(EntityFlag::MAX_FLAGS)>;

	// Every entity is going to have this as a component
	struct EntityMetadata
	{
		RYU_ENABLE_REFLECTION(EntityMetadata);
	
	public:
		explicit EntityMetadata()
			: m_uuidBytes(Utils::UUID::GenerateBytes())
		{
		}

		explicit EntityMetadata(const std::string& prettyUUIDStr)
		{
			m_uuidBytes = prettyUUIDStr.empty() ? Utils::UUID::GenerateBytes() : Utils::UUID::FromPrettyStringToBytes(prettyUUIDStr);
		}

		[[nodiscard]] inline const Utils::UUID::Type GetUUID() const { return Utils::UUID::FromBytes(m_uuidBytes); }
		[[nodiscard]] inline const std::string& GetUUIDBytes() const { return m_uuidBytes; }
		[[nodiscard]] inline std::string GetUUIDPretty() const { return Utils::UUID::FromBytesToPrettyString(m_uuidBytes); }

		std::string Name;
		EntityFlags Flags;

	private:
		// Maybe change this to char[16]?
		std::string m_uuidBytes;
	};
}

RYU_REFLECTED_CLASS(
	Ryu::Game::EntityMetadata,
	"EntityMetadata",
	RYU_CLASS_ATTRIB(Ryu::Game::EntityMetadata, Name),
	RYU_CLASS_ATTRIB(Ryu::Game::EntityMetadata, Flags),
	RYU_CLASS_ATTRIB(Ryu::Game::EntityMetadata, m_uuidBytes))

#include "EntityMetadata.inl"
