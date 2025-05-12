#pragma once
#include "Common/API.h"
#include "Common/ObjectMacros.h"

namespace Ryu::App { class Application; }

namespace Ryu::Engine
{
	class RYU_API IGameModule
	{
	public:
		virtual ~IGameModule() = default;

		virtual App::Application* CreateApplication() = 0;
		virtual constexpr const char* GetName() const = 0;
	};

	using CreateGameModuleFunc = IGameModule*(*)();
}

#if defined(RYU_EXPORTS)
#define RYU_GAME_API RYU_API
#else
#define RYU_GAME_API __declspec(dllexport)
#endif

#if defined(RYU_GAME_AS_DLL)
#define RYU_IMPLEMENT_GAME_MODULE(GameModuleClass)                       \
    extern "C" RYU_GAME_API Ryu::Engine::IGameModule* CreateGameModule() \
	{                                                                    \
        return new GameModuleClass();                                    \
    }
#else
#define RYU_IMPLEMENT_GAME_MODULE(GameModuleClass)
#endif
