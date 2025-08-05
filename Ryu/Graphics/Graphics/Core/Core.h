#pragma once
#include "Graphics/Core/DX12.h"

namespace Ryu::Gfx
{
	void Init();
	void Shutdown();


	const ComPtr<DX12::Device>& GetDevice();
	const ComPtr<DXGI::Factory>& GetFactory();
}
