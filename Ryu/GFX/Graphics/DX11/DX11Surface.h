#pragma once
#include "Graphics/ISurface.h"

namespace Ryu::Graphics::DX11
{
	class DX11Surface : public ISurface
	{
	public:
		explicit DX11Surface(App::WindowBase* window);
		~DX11Surface();

		void CreateSwapChain(IDXGIFactory7* factory);

		void Present() const override;

	private:
		IDXGISwapChain4* m_swapChain{ nullptr };
	};
}
