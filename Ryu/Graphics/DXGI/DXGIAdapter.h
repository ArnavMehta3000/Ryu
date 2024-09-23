#pragma once
#include <Graphics/Common.h>

namespace Ryu::Graphics
{
	class DXGIAdapter : public ComPtr<IDXGIAdapter4>
	{
	public:
		DXGIAdapter() = default;
		explicit DXGIAdapter(IDXGIAdapter4* adapter)
			: ComPtr<IDXGIAdapter4>(adapter)
		{
		}
	};
}
