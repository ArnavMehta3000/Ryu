#include "IDX11Object.h"
#include "Graphics/Shared/Logging.h"

namespace Ryu::Graphics
{
	void IDX11Object::SetName(auto object, std::string_view name)
	{
		DX11_NAME_OBJECT(object, name);
	}
}
