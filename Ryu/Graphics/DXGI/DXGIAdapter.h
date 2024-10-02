#pragma once
#include <Graphics/Common.h>

namespace Ryu::Graphics
{
	class DXGIAdapter : public ComPtr<IDXGIAdapter4>
	{
	public:
		DXGIAdapter() = default;
		DXGIAdapter(InterfaceType* adapter)
			: ComPtr(adapter)
		{
		}
	};
}
