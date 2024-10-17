#pragma once
#include "Common/Common.h"
#include "App/WindowBase.h"

namespace Ryu::Graphics
{
	class Surface {};

	struct RenderSurface
	{
		App::WindowBase* Window;
		Surface Surface;
	};

	enum class API
	{
		DirectX12 = 0,
	};

	bool Init(API api);
	void Shutdown();
	void Render();
}
