#pragma once
#include "Game/Core/GameServices.h"
#include "Game/IGameModule.h"
#include "Testbench/Game/TestbenchGameState.h"

#if defined (RYU_HOT_RELOAD)
#include "Engine/HotReload/GameModuleABI.h"
#endif

#include <memory>

class TestbenchModule final : public Ryu::Game::IGameModule
{
	static constexpr auto AppName = "Testbench App";
	RYU_DECLARE_HOT_RELOAD_MODULE_GAME_STATE(TestbenchGameState)
public:
	std::shared_ptr<Ryu::App::App> CreateApplication(std::shared_ptr<Ryu::Window::Window> window) override;
	constexpr const char* GetName() const override { return AppName; }
	
	static RyuModuleInfo GetModuleInfo();
	static bool Initialize(RyuGameState state);
	static void Tick(RyuGameState state, RyuTickContext ctx);
	static void Shutdown(RyuGameState state);
	static RyuWorldManager GetWorldManager(RyuGameState state);
	static RyuSerializedState SerializeState(RyuGameState) { return {}; }
	static bool DeserializeState(RyuGameState, RyuSerializedState) { return true; }
	static void FreeSerializedState(RyuSerializedState) {}
};
