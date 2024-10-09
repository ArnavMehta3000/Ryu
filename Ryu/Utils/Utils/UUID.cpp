#include "UUID.h"

namespace Ryu::Utils
{
	UUID::Generator s_uuidGenerator;
	
	UUID::Type UUID::Generate()
	{
		return s_uuidGenerator.getUUID();
	}

}
