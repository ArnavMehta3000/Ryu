#include "Game/Core/GameServices.h"
#include "Game/IGameModule.h"
#include "Testbench/TestbenchApp.h"
#include "Testbench/TestbenchGame.h"

#if defined (RYU_HOT_RELOAD)
#include "Engine/HotReload/GameModuleABI.h"
#endif

constexpr auto AppName = "Testbench App";

#if !defined (RYU_HOT_RELOAD)  // This is used when statically linking (no-hotreload)
class TestbenchModule final : public Ryu::Game::IGameModule
{
public:
	std::shared_ptr<Ryu::App::App> CreateApplication(std::shared_ptr<Ryu::Window::Window> window) override
	{
		return std::make_shared<TestbenchApp>(window);
	}

	constexpr const char* GetName() const override
	{
		return AppName;
	}
};

RYU_IMPLEMENT_GAME_MODULE(TestbenchModule)

#else

using namespace Ryu;
using namespace Ryu::Game;

class TestbenchHotReloadModule
{
public:
	static RyuModuleInfo GetModuleInfo()
	{
		return RyuModuleInfo
		{
			.Name                  = AppName,
			.Author                = "Batman",
			.Version               = {1, 0, 0 },
			.RequiredEngineVersion = { 1, 0, 0 }
		};
	}

	static bool LoadModule(RyuServices services)
	{
		s_services = std::make_unique<GameServices>(services);
		Game::Internal::g_services = s_services.get();  // Set thread-local

		RYU_GAME_LOG_INFO("Testbench module loaded");
		return true;
	}

	static void UnloadModule()
	{
		RYU_GAME_LOG_INFO("Testbench module unloading");

		Game::Internal::g_services = nullptr;
		s_services.reset();
	}

	static RyuGameState CreateGameState()
	{
		return new TestbenchGame();
	}

	static void DestroyGameState(RyuGameState state)
	{
		delete static_cast<TestbenchGame*>(state);
	}

	static bool Initialize(RyuGameState state)
	{
		return static_cast<TestbenchGame*>(state)->Initialize();
	}

	static void Tick(RyuGameState state, RyuTickContext ctx)
	{
		static_cast<TestbenchGame*>(state)->Tick(ctx.DeltaTime);
	}

	static void Shutdown(RyuGameState state)
	{
		static_cast<TestbenchGame*>(state)->Shutdown();
	}

	static RyuWorld GetActiveWorld(RyuGameState state)
	{
		return static_cast<TestbenchGame*>(state)->GetActiveWorld();
	}

	static RyuSerializedState SerializeState(RyuGameState)
	{
		//static SerializationBuffer buffer;  // Static to keep memory alive
		//buffer.Clear();

		//buffer.Write(GAME_STATE_VERSION);
		//static_cast<TestbenchGame*>(state)->Serialize(buffer);

		//return RyuSerializedState
		//{
		//	.Data = buffer.Data(),
		//	.Size = buffer.Size(),
		//	.Version = GAME_STATE_VERSION
		//};

		// Pass empty state for now
		return RyuSerializedState
		{
			.Data = nullptr,
			.Size = 0,
			.Version = 0
		};
	}

	static bool DeserializeState(RyuGameState, RyuSerializedState)
	{
		RYU_TODO("Sort out serialization")
		//SerializationBuffer buffer;
		//buffer.SetData(data.Data, data.Size);

		//uint32_t version;
		//if (!buffer.Read(version))
		//	return false;

		// Handle version migration if needed
		// if (version != GAME_STATE_VERSION)
		// {
		// 	RYU_GAME_LOG_WARN("State version mismatch: {} vs {}", version, GAME_STATE_VERSION);
		// 	// Could implement migration here
		// 	return false;
		// }

		//return static_cast<TestbenchGame*>(state)->Deserialize(buffer);
		return true;
	}

	static void FreeSerializedState(RyuSerializedState)
	{
		// Using static buffer, nothing to free
	}

private:
	static inline std::unique_ptr<GameServices> s_services;

};

RYU_IMPLEMENT_HOT_RELOAD_MODULE(TestbenchHotReloadModule);
#endif
