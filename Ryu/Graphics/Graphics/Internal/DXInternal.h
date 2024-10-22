#pragma once
#include "Graphics/Common.h"

namespace Ryu::Graphics
{
	enum class API;

	namespace Internal
	{
		bool Init(API api);
		void Shutdown();

		IDXGIFactory7* const GetFactory();
		IDXGIAdapter4* const GetMainAdapter();
		D3D_FEATURE_LEVEL GetMaxFeatureLevel();
	}
}
