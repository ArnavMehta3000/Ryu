#pragma once
#include <Graphics/Common.h>

namespace Ryu::Graphics
{
	class DXGIAdapter;

	class DXGIFactory : public ComPtr<IDXGIFactory7>
	{
	public:
		DXGIFactory() = default;
		DXGIFactory(InterfaceType* ptr);
		
		static NODISCARD CreateResult<DXGIFactory> Create();

		DXGIAdapter GetAdapter() const;
		bool HasTearingSupport() const;
	};
}
