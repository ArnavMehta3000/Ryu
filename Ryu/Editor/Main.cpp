#include <Editor/Application/EditorApp.h>
#include <Editor/Internal/Log.h>
#include <Engine/Engine.h>

int WINAPI wWinMain(
	_In_     MAYBE_UNUSED HINSTANCE hInstance,
	_In_opt_ MAYBE_UNUSED HINSTANCE hPrevInstance,
	_In_     MAYBE_UNUSED LPWSTR    lpCmdLine,
	_In_     MAYBE_UNUSED int       nCmdShow)
{
	RYU_EDITOR_INITIALIZE_LOGGER("Editor");

	// Engine is loaded as a DLL in the Editor
	{
		Ryu::DllLoader engineLoader;
		RYU_EDITOR_ASSERT(engineLoader.LoadDLL("RyuEngine.dll"), "Failed to load RyuEngine.dll");

		auto getEngineInstance = engineLoader.GetProcAddressFunc<Ryu::GetEngineInstance_f>("GetEngineInstance");
		RYU_EDITOR_ASSERT(getEngineInstance, "Failed to get GetEngineInstance function from RyuEngine.dll");

		Ryu::Engine& engine = getEngineInstance();

		engine.CreateApplication<Ryu::Editor::EditorApp>(hInstance);
		engine.Run();
	}

	RYU_EDITOR_SHUTDOWN_LOGGER();

	return 0;
}