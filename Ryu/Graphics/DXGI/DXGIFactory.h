#pragma once
#include <Graphics/Common.h>

namespace Ryu::Graphics
{
	class DXGIAdapter;

	class DXGIFactory : public ComPtr<IDXGIFactory7>
	{
	public:
		DXGIFactory() = default;
		explicit DXGIFactory(InterfaceType* ptr);
		static NODISCARD CreateResult<InterfaceType*> Create();

		void GetAdapter(DXGIAdapter& outAdapter);
		bool HasTearingSupport() const;
	};
}
