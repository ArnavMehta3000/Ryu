#pragma once

extern "C"
{
	using RyuServices  = void*;
	using RyuGameState = void*;
	using RyuWorld     = void*;
	using RyuEntity    = void*;

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
		const byte* Data;
		u64 Size;
		u32 Version;  // State format version
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

	using FnGetActiveWorld      = RyuWorld(*)(RyuGameState state);

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
		FnGetActiveWorld      GetActiveWorld;

		FnSerializeState      SerializeState;
		FnDeserializeState    DeserializeState;
		FnFreeSerializedState FreeSerializedState;

		FnOnEditorAttach      OnEditorAttach;
		FnOnEditorDetach      OnEditorDetach;
		FnOnEditorRender      OnEditorRender;
	};

	__declspec(dllexport) RyuGameModuleAPI* RyuGetGameModuleAPI();
}
