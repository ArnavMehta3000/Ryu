#pragma once
#include <Graphics/Common.h>

namespace Ryu::Graphics
{
	class DXGIDebug : public ComPtr<IDXGIDebug1>
	{
	public:
		DXGIDebug();
		void ReportLiveObjects();
	};
}
