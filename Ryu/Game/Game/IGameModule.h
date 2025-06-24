#pragma once
#include "Common/API.h"
#include "Common/ObjectMacros.h"
#include <memory>

namespace Ryu::App { class App; }
namespace Ryu::Window { class Window; }

namespace Ryu::Game
{
	class RYU_API IGameModule
	{
	public:
		virtual ~IGameModule() = default;

		virtual std::shared_ptr<App::App> CreateApplication(std::shared_ptr<Window::Window> window) = 0;
		virtual constexpr const char* GetName() const = 0;
	};

    extern "C" Ryu::Game::IGameModule* CreateGameModule();

#if defined(RYU_WITH_EDITOR)
    // Editor build: Game implements this function
#define RYU_IMPLEMENT_GAME_MODULE(GameModuleClass)        \
    extern "C" Ryu::Game::IGameModule* CreateGameModule() \
    {                                                     \
        return new GameModuleClass();                     \
    }
#else
    // Standalone build: Game links statically, needs definition
#define RYU_IMPLEMENT_GAME_MODULE(GameModuleClass)\
    Ryu::Game::IGameModule* CreateGameModule()    \
    {                                             \
        return new GameModuleClass();             \
    }
#endif
}
