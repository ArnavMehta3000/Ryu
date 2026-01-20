#include "Game/IGameModule.h"
#include "Engine/HotReload/GameModuleABI.h"
#include "Game/Core/GameServices.h"
#include "Testbench/TestbenchApp.h"
#include "Testbench/TestbenchGame.h"

class TestModule final : public Ryu::Game::IGameModule
{
public:
	std::shared_ptr<Ryu::App::App> CreateApplication(std::shared_ptr<Ryu::Window::Window> window) override
	{
		return std::make_shared<TestbenchApp>(window);
	}

	constexpr const char* GetName() const override
	{
		return "Testbench App";
	}
};

RYU_IMPLEMENT_GAME_MODULE(TestModule)

using namespace Ryu;
using namespace Ryu::Game;

static GameServices* s_services = nullptr;

// API implementation

static RyuModuleInfo GetModuleInfo()
{
	return RyuModuleInfo
	{
		.Name                  = "Testbench",
		.Author                = "Batman",
		.Version               = {1, 0, 0 },
		.RequiredEngineVersion = { 1, 0, 0 }
	};
}

static bool LoadModule(RyuServices services)
{
	s_services = new GameServices(services);
	Game::Internal::g_services = s_services;  // Set thread-local

	RYU_GAME_LOG_INFO("Testbench module loaded");
	return true;
}

static void UnloadModule()
{
	RYU_GAME_LOG_INFO("Testbench module unloading");

	Game::Internal::g_services = nullptr;
	delete s_services;
	s_services = nullptr;
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

static RyuSerializedState SerializeState(RyuGameState state)
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
		.Data    = nullptr,
		.Size    = 0,
		.Version = 0
	};
}

RYU_TODO("Sort out seerialization")

static bool DeserializeState(RyuGameState state, RyuSerializedState data)
{
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

static void FreeSerializedState(RyuSerializedState data)
{
	// Using static buffer, nothing to free
}

static RyuGameModuleAPI s_api =
{
	.GetModuleInfo       = GetModuleInfo,
	.LoadModule          = LoadModule,
	.UnloadModule        = UnloadModule,
	.CreateGameState     = CreateGameState,
	.DestroyGameState    = DestroyGameState,
	.Initialize          = Initialize,
	.Tick                = Tick,
	.Shutdown            = Shutdown,
	.GetActiveWorld      = GetActiveWorld,
	.SerializeState      = SerializeState,
	.DeserializeState    = DeserializeState,
	.FreeSerializedState = FreeSerializedState,
	.OnEditorAttach      = nullptr,
	.OnEditorDetach      = nullptr,
	.OnEditorRender      = nullptr
};

extern "C" __declspec(dllexport) RyuGameModuleAPI* RyuGetGameModuleAPI()
{
	return &s_api;
}
