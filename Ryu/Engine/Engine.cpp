#include "Engine.h"
#include <Engine/Globals.h>

namespace Ryu
{
	Engine* g_engine{ nullptr };

	RYU_API bool InitializeEngineInstance(Engine* engine)
	{
		g_engine = engine;
		return true;
	}

}
