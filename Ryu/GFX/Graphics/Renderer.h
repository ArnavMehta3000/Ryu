#pragma once

namespace Ryu::Graphics
{
	enum class API;

	bool Init(API api);
	void Shutdown();
}
