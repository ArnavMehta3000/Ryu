#pragma once
#include <Graphics/Common.h>

namespace Ryu::Graphics
{
	class DXGIAdapter;

	class DXGIFactory : public ComPtr<IDXGIFactory7>
	{
	public:
		DXGIFactory();
		DXGIAdapter GetAdapter();
		bool HasTearingSupport();
	};
}
