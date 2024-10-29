#pragma once

namespace Ryu::Graphics
{
	enum class API;

	bool IsInitialized();
	bool Init(API api);
	void Shutdown();
}
