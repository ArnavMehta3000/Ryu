#pragma once
#include <string_view>

namespace Ryu::Graphics
{
	class IDX12Object
	{
	public:
		void SetName(auto object, std::string_view name);
	};
}
