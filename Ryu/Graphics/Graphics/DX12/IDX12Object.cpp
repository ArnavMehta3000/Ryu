#include "IDX12Object.h"
#include "Graphics/Shared/Logging.h"
#include "Utils/StringConv.h"
#include <string>

namespace Ryu::Graphics
{
	void IDX12Object::SetName(auto object, std::string_view name)
	{
		DX12_NAME_OBJECT(object, Utils::ToWideStr(name));
	}
}
