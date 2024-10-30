#include <Windows.h>
#include "Test.h"
#include "Graphics/Config.h"
#include "Plugin/PluginLoader.h"
#include "../Plugins/SamplePlugin/SamplePlugin/SamplePlugin.h"

int WINAPI wWinMain(
	_In_     MAYBE_UNUSED HINSTANCE hInstance,
	_In_opt_ MAYBE_UNUSED HINSTANCE hPrevInstance,
	_In_     MAYBE_UNUSED LPWSTR    lpCmdLine,
	_In_     MAYBE_UNUSED int       nCmdShow)
{
	using namespace Ryu;

	// Set the working API
	Graphics::GraphicsConfig::Get().GraphicsAPI = Graphics::API::DirectX12 ;

	// Creating the engine object initializes all core subsystems
	Engine::Engine engine;
	Logging::SetUpDefaultLogger();


	Plugin::PluginLoader loader;
	
	// Dynamically load the plugin
	{
		auto plugin = loader.Load("SamplePlugin");
		int x = 0;
	}
	
	// Statically load the plugin
	//{
	//	auto plugin = loader.Load<Sample::SamplePlugin>();
	//	plugin->SomeData = 20;
	//}


	engine.SetCommandLine(lpCmdLine);
	engine.SetApp(std::make_shared<TestApp>());
	engine.Run();

	return 0;
}
