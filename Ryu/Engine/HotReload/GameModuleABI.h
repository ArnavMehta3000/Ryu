#pragma once

extern "C"
{
	using RyuServices     = void*;
	using RyuGameState    = void*;
	using RyuWorldManager = void*;
	using RyuEntity       = void*;

	struct RyuModuleVersion
	{
		u32 Major;
		u32 Minor;
		u32 Patch;
	};

	struct RyuModuleInfo
	{
		const char* Name;
		const char* Author;
		RyuModuleVersion Version;
		RyuModuleVersion RequiredEngineVersion;
	};

	struct RyuSerializedState
	{
		const byte* Data = nullptr;
		u64 Size = 0;
		u32 Version = 0;  // State format version
	};

	struct RyuTickContext
	{
		f32 DeltaTime;
		f32 TotalTime;
		u64 FrameNumber;
		bool IsPaused;
	};

	/// --------- GAME MODULE API --------- 

	using FnGetModuleInfo       = RyuModuleInfo(*)();
	using FnLoadModule          = bool(*)(RyuServices services);
	using FnUnloadModule        = void (*)();

	using FnCreateGameState     = RyuGameState(*)();
	using FnDestroyGameState    = void(*)(RyuGameState state);

	using FnSerializeState      = RyuSerializedState(*)(RyuGameState state);
	using FnDeserializeState    = bool(*)(RyuGameState state, RyuSerializedState data);
	using FnFreeSerializedState = void(*)(RyuSerializedState data);

	using FnInitialize          = bool(*)(RyuGameState);
	using FnTick                = void(*)(RyuGameState state, RyuTickContext ctx);
	using FnShutdown            = void(*)(RyuGameState state);

	using FnGetWorldManager     = RyuWorldManager(*)(RyuGameState state);

	using FnOnEditorAttach      = void(*)(RyuGameState state);
	using FnOnEditorDetach      = void(*)(RyuGameState state);
	using FnOnEditorRender      = void(*)(RyuGameState state);


	struct RyuGameModuleAPI
	{
		FnGetModuleInfo       GetModuleInfo;
		FnLoadModule          LoadModule;
		FnUnloadModule        UnloadModule;
		FnCreateGameState     CreateGameState;
		FnDestroyGameState    DestroyGameState;
		FnInitialize          Initialize;
		FnTick                Tick;
		FnShutdown            Shutdown;
		FnGetWorldManager     GetWorldManager;

		FnSerializeState      SerializeState;
		FnDeserializeState    DeserializeState;
		FnFreeSerializedState FreeSerializedState;

		FnOnEditorAttach      OnEditorAttach;
		FnOnEditorDetach      OnEditorDetach;
		FnOnEditorRender      OnEditorRender;
	};

	__declspec(dllexport) RyuGameModuleAPI* RyuGetGameModuleAPI();
}


namespace Ryu::Engine
{
	// A concept to check if a hot-reloadable module has static functions matching the API
	template <typename T>
	concept IsHotReloadModule = requires(
		RyuServices services, RyuGameState state,
		RyuTickContext ctx, RyuSerializedState data)
	{
		{ T::GetModuleInfo()               } -> IsSame<RyuModuleInfo>;
		{ T::LoadModule(services)          } -> IsSame<bool>;
		{ T::UnloadModule()                } -> IsSame<void>;
		{ T::CreateGameState()             } -> IsSame<RyuGameState>;
		{ T::DestroyGameState(state)       } -> IsSame<void>;
		{ T::Initialize(state)             } -> IsSame<bool>;
		{ T::Tick(state, ctx)              } -> IsSame<void>;
		{ T::Shutdown(state)               } -> IsSame<void>;
		{ T::GetWorldManager(state)        } -> IsSame<RyuWorldManager>;

		{ T::SerializeState(state)         } -> IsSame<RyuSerializedState>;
		{ T::DeserializeState(state, data) } -> IsSame<bool>;
		{ T::FreeSerializedState(data)     } -> IsSame<void>;
	};

	template <typename T>
	concept ModuleHasEditorSupport = requires
	{
		{ T::OnEditorAttach() } -> IsSame<void>;
		{ T::OnEditorDetach() } -> IsSame<void>;
		{ T::OnEditorRender() } -> IsSame<void>;
	};

	template <typename T>
	constexpr bool ModuleHasEditorAttach = requires { { T::OnEditorAttach() } -> IsSame<void>; };

	template <typename T>
	constexpr bool ModuleHasEditorDetach = requires { { T::OnEditorDetach() } -> IsSame<void>; };

	template <typename T>
	constexpr bool ModuleHasEditorRender = requires { { T::OnEditorRender() } -> IsSame<void>; };

	template <typename T>
	RyuGameModuleAPI CreateModuleAPI()
	{
		RyuGameModuleAPI api =
		{
			.GetModuleInfo       = T::GetModuleInfo,
			.LoadModule          = T::LoadModule,
			.UnloadModule        = T::UnloadModule,
			.CreateGameState     = T::CreateGameState,
			.DestroyGameState    = T::DestroyGameState,
			.Initialize          = T::Initialize,
			.Tick                = T::Tick,
			.Shutdown            = T::Shutdown,
			.GetWorldManager     = T::GetWorldManager,
			.SerializeState      = T::SerializeState,
			.DeserializeState    = T::DeserializeState,
			.FreeSerializedState = T::FreeSerializedState,
			.OnEditorAttach      = nullptr,
			.OnEditorDetach      = nullptr,
			.OnEditorRender      = nullptr,
		};

		if constexpr (ModuleHasEditorAttach<T>) 
		{
			api.OnEditorAttach = T::OnEditorAttach;
		}
		
		if constexpr (ModuleHasEditorDetach<T>) 
		{
			api.OnEditorDetach = T::OnEditorDetach;
		}
		
		if constexpr (ModuleHasEditorRender<T>) 
		{
			api.OnEditorRender = T::OnEditorRender;
		}

		return api;
	}
}

// This macro will add the boiler-plate functions that are required for setup
#define RYU_DECLARE_HOT_RELOAD_MODULE_GAME_STATE(GameStateClass)                 \
	private:                                                                     \
		static inline std::unique_ptr<::Ryu::Game::GameServices> s_services;     \
	                                                                             \
	public:                                                                      \
		static bool LoadModule(RyuServices services)                             \
		{                                                                        \
			s_services = std::make_unique<::Ryu::Game::GameServices>(services);  \
			::Ryu::Game::Internal::g_services = s_services.get();                \
			return true;                                                         \
		}                                                                        \
																			     \
		static void UnloadModule()                                               \
		{                                                                        \
			::Ryu::Game::Internal::g_services = nullptr;                         \
			s_services.reset();                                                  \
		}                                                                        \
																			     \
		static RyuGameState CreateGameState()                                    \
		{                                                                        \
			return new GameStateClass();                                         \
		}                                                                        \
                                                                                 \
		static void DestroyGameState(RyuGameState state)                         \
		{                                                                        \
			delete static_cast<GameStateClass*>(state);                          \
		}                                                                    


#define RYU_IMPLEMENT_HOT_RELOAD_MODULE(ModuleClassName)                      \
    static_assert(::Ryu::Engine::IsHotReloadModule<ModuleClassName>);         \
    extern "C" __declspec(dllexport) RyuGameModuleAPI* RyuGetGameModuleAPI()  \
    {                                                                         \
        static RyuGameModuleAPI s_api =                                       \
            ::Ryu::Engine::CreateModuleAPI<ModuleClassName>();                \
        return &s_api;                                                        \
    }
