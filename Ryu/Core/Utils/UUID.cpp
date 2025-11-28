#include "UUID.h"

namespace Ryu::Utils
{
	UUID::Generator s_uuidGenerator;
	
	UUID::Type UUID::Generate()
	{
		return s_uuidGenerator.getUUID();
	}

	std::string UUID::GenerateBytes()
	{
		Type id = Generate();
		return id.bytes();
	}

	std::string UUID::GenerateString()
	{
		Type id = Generate();
		return id.str();
	}

	UUID::Type UUID::FromString(const std::string& str)
	{
		return UUIDv4::UUID::fromStrFactory(str);
	}

	UUID::Type UUID::FromBytes(const std::string& str)
	{
		return Type(str);
	}

	std::string UUID::FromBytesToPrettyString(const std::string& str)
	{
		Type id = FromBytes(str);
		return id.str();
	}

	std::string UUID::FromPrettyStringToBytes(const std::string& str)
	{
		Type id = FromString(str);
		return id.bytes();
	}
}
