#pragma once
#include <Graphics/Common.h>

namespace Ryu::Graphics
{
	class DXGIOutput : public ComPtr<IDXGIOutput6>
	{
	public:
		DXGIOutput(IDXGIOutput6* ptr) : ComPtr(ptr) {}
		DXGIOutput() = default;
	};
}
