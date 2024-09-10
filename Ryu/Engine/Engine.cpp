#include "Engine.h"
#include <Engine/Log.h>
#include <Plugins/Engine/RyuInput/InputSystem.h>
#include <filesystem>
#include <chrono>

namespace Ryu
{
	RYU_API Engine& GetEngineInstance()
	{
		return Engine::Get();
	}

	Engine::Engine()
		: m_application(nullptr)
	{
	}

	Engine::~Engine()
	{
		m_application.reset();
	}

	bool Engine::PreInit()
	{
		RYU_ENGINE_DEBUG("Pre-initializing engine");

		LoadPlugins();

		InitializePlugins(PluginLoadOrder::PreInit);

		RYU_ENGINE_TRACE("Finished pre-initializing engine");
		return true;
	}

	bool Engine::Init()
	{
		RYU_ENGINE_DEBUG("Initializing engine");

		if (!m_application->OnInit())
		{
			RYU_ENGINE_FATAL("Failed to initialize application!");
			return false;
		}

		RYU_ENGINE_TRACE("Finished initializing engine");
		return true;
	}

	bool Engine::PostInit()
	{
		RYU_ENGINE_DEBUG("Post-initializing engine");

		InitializePlugins(PluginLoadOrder::PostInit);
		SetInputCallbacks();

		RYU_ENGINE_TRACE("Finished post-initializing engine");
		return true;
	}

	void Engine::SetInputCallbacks()
	{
		if (Input::InputSystem* input = m_pluginManager.GetPlugin<Input::InputSystem>("RyuInput"))
		{
			Input::InputCallbacks callbacks;
			callbacks.OnKeyDown = [this](const auto& event) {m_application->OnEvent(event); };
			callbacks.OnKeyUp = [this](const auto& event) {m_application->OnEvent(event); };
				
			input->SetInputCallbacks(callbacks);
		}
	}

	void Engine::AddPlugins(std::initializer_list<std::string> plugins)
	{
		for (auto& plugin : plugins)
		{
			// Add if plugin exists in engine plugins directory
			std::filesystem::path path = std::filesystem::current_path();
			path.append(ENGINE_PLUGINS_PATH).append(plugin + ".dll");

			if (std::filesystem::exists(path))
			{
				m_pluginManager.AddPlugin(plugin, path.string());
			}
			else
			{
				RYU_ENGINE_ERROR("Plugin {{}} not found in directory {}", plugin, path.string());
			}
		}
	}

	void Engine::Run()
	{
		RYU_ENGINE_INFO("Starting engine");

		// Set application to be running
		m_application->m_isRunning = true;

		PreInit();
		Init();
		PostInit();

		using ClockType = std::chrono::steady_clock;

		ClockType clock;
		std::chrono::time_point<ClockType> lastTime;
		auto timePoint1 = clock.now();
		auto timePoint2 = timePoint1;
		f32 deltaTime = 0.001f;

		RYU_ENGINE_INFO("Engine ready to run");

		while (m_application->m_window.IsOpen() && m_application->IsRunning())
		{
			Tick(deltaTime);

			timePoint2 = clock.now();
			std::chrono::duration<f32> dt = timePoint2 - timePoint1;
			timePoint1 = timePoint2;
			deltaTime = dt.count();
		}

		Shutdown();
	}

	void Engine::Tick(MAYBE_UNUSED f32 dt)
	{
		m_application->m_window.PumpMessages();

		m_application->OnUpdate(dt);
		m_application->OnRender();
	}

	void Engine::Shutdown()
	{
		RYU_ENGINE_DEBUG("Shutting down engine");
		DestroyPlugins();
		m_application->OnShutdown();
		RYU_ENGINE_TRACE("Finished shutting down engine");
	}

	void Engine::DestroyPlugins()
	{
		RYU_ENGINE_INFO("Unloading plugins");

		PluginManager::PluginMap& map = m_pluginManager.GetPluginsMap();
		const size_t count = map.size();
		for (auto& [name, plugin] : map)
		{
			if (plugin.Plugin)
			{
				plugin.Plugin->Shutdown();
			}


			if (plugin.DestroyPluginFunc)
			{
				plugin.DestroyPluginFunc(plugin.Plugin);
				plugin.DestroyPluginFunc = nullptr;
				plugin.Plugin = nullptr;

				RYU_ENGINE_DEBUG("Destroyed plugin: {}", name);
			}
		}

		RYU_ENGINE_TRACE("Finished unloading {} plugins", count);
	}

	void Engine::LoadPlugins()
	{
		PluginManager::PluginMap& map = m_pluginManager.GetPluginsMap();
		
		for (auto& [name, plugin] : map)
		{
			// Create dll loader
			if (!plugin.DLL)
			{
				plugin.DLL = std::make_unique<DllLoader>();
			}

			// Load dll
			if (plugin.DLL->LoadDLL(plugin.PluginPath))
			{
				RYU_ENGINE_TRACE("Loaded plugin DLL: {}", name);
			}
			else
			{
				RYU_ENGINE_ERROR("Failed to load plugin DLL: {}", name);
			}

			// Create plugin
			if (CreatePlugin_f createPlugin = plugin.DLL->GetProcAddressFunc<CreatePlugin_f>("CreatePlugin"))
			{
				plugin.Plugin = createPlugin();
				RYU_ENGINE_DEBUG("Created plugin: {}", name);
			}
			else
			{
				RYU_ENGINE_ERROR("Failed to create plugin: {}", name);
			}

			// Get destroy plugin function
			if (DestroyPlugin_f destroyPlugin = plugin.DLL->GetProcAddressFunc<DestroyPlugin_f>("DestroyPlugin"))
			{
				plugin.DestroyPluginFunc = destroyPlugin;
				RYU_ENGINE_TRACE("Cached DestroyPlugin function for plugin: {}", name);
			}
			else
			{
				RYU_ENGINE_ERROR("Failed to destroy plugin: {}", name);
			}
		}
	}

	void Engine::InitializePlugins(Ryu::PluginLoadOrder order)
	{
		const PluginAPI api
		{
			.Window = m_application->GetWindow()
		};

		PluginManager::PluginMap& map = m_pluginManager.GetPluginsMap();
		for (auto& [name, plugin] : map)
		{
			if (plugin.Plugin && plugin.Plugin->GetLoadOrder() == order)
			{
				if (!plugin.Plugin->Initialize(api))
				{
					RYU_ENGINE_FATAL("Plugin initialization failed for: {}", name);
				}
			}
		}
	}
}
