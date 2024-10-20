#pragma once

namespace Ryu::App { class WindowBase; }

namespace Ryu::Graphics
{
	enum class API
	{
		DirectX12 = 0,
	};

	bool Init(API api);
	void Shutdown();
}
