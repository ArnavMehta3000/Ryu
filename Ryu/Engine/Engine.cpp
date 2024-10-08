#include "Engine.h"
#include <Core/Utils/StringConv.h>
#include <Engine/Input/InputSystem.h>
#include <Engine/Internal/Log.h>
#include <filesystem>
#include <chrono>
#include <nlohmann/json.hpp>

namespace Ryu
{
	namespace
	{
		static constexpr byte g_engineConfig[] =
		{
			#include <EngineConfig.json.h>
		};
	}

	RYU_API Engine& GetEngineInstance()
	{
		return Engine::Get();
	}

	Engine::Engine()
		: m_application(nullptr)
		, m_renderAPI(nullptr)
		, m_engineServices(nullptr)
	{
		m_engineServices = std::make_shared<ServiceLocator>();
	}

	Engine::~Engine() = default;

	bool Engine::PreInit()
	{
		RYU_ENGINE_DEBUG("Pre-initializing engine");

		// Set resize callback
		m_application->m_window.SetResizeCallback(
			[this](i32 width, i32 height)
			{
				OnResize(width, height);
			});

		LoadConfig();
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

		HWND hWnd = m_application->GetWindow();
		const Window::Config& config = m_application->GetWindow().GetConfig();

		m_renderAPI = std::make_unique<Graphics::RenderAPI>();
		m_renderAPI->Initialize(hWnd, config.Width, config.Height);

		m_engineServices->RegisterService(std::make_shared<InputSystem>(hWnd));

		InitializePlugins(PluginLoadOrder::PostInit);
		AddInputCallbacks();

		RYU_ENGINE_TRACE("Finished post-initializing engine");
		return true;
	}

	void Engine::AddInputCallbacks()
	{
		if (auto input = m_engineServices->ResolveService<IInputService>())
		{
			InputCallbacks callbacks;
			callbacks.OnKeyDown         = [this](const auto& event) { m_application->OnEvent(event); };
			callbacks.OnKeyUp           = [this](const auto& event) { m_application->OnEvent(event); };
			callbacks.OnMouseButtonUp   = [this](const auto& event) { m_application->OnEvent(event); };
			callbacks.OnMouseButtonDown = [this](const auto& event) { m_application->OnEvent(event); };
			callbacks.OnMouseDblClick   = [this](const auto& event) { m_application->OnEvent(event); };
			callbacks.OnMouseMove       = [this](const auto& event) { m_application->OnEvent(event); };
			callbacks.OnMouseMoveRaw    = [this](const auto& event) { m_application->OnEvent(event); };
			callbacks.OnMouseWheel      = [this](const auto& event) { m_application->OnEvent(event); };

			// Add application callbacks to input system
			input->AddInputCallbacks(callbacks);
		}
	}

	void Engine::AddPlugins(const std::string& name)
	{
		// Add if plugin exists in engine plugins directory
		std::filesystem::path path = std::filesystem::current_path();
		path.append(name + ".dll");

		PluginEntry entry;
		entry.PluginPath = path.string();

		if (std::filesystem::exists(path))
		{
			m_pluginManager.AddPlugin(name, entry);
		}
		else
		{
			RYU_ENGINE_ERROR("Plugin {{}} not found in directory {}", name, path.string());
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

	void Engine::Tick(MAYBE_UNUSED const f32 dt)
	{
		m_application->m_window.PumpMessages();

		TickPlugins(PluginTickOrder::PreUpdate, dt);
		m_application->OnUpdate(dt);
		TickPlugins(PluginTickOrder::PostUpdate, dt);

		RenderPlugins(PluginRenderOrder::PreRender, dt);
		m_application->OnRender(dt);
		RenderPlugins(PluginRenderOrder::PostRender, dt);
		m_renderAPI->Present();
	}

	void Engine::TickPlugins(PluginTickOrder order, const f32 dt)
	{
		auto& map = m_pluginManager.GetPluginsMap();
		for (auto& [name, plugin] : map)
		{
			if (plugin.Plugin && plugin.Plugin->GetPluginData().TickOrder == order)
			{
				plugin.Plugin->OnUpdate(dt);
			}
		}
	}

	void Engine::RenderPlugins(PluginRenderOrder order, const f32 dt)
	{
		auto& map = m_pluginManager.GetPluginsMap();
		for (auto& [name, plugin] : map)
		{
			if (plugin.Plugin && plugin.Plugin->GetPluginData().RenderOrder == order)
			{
				plugin.Plugin->OnRender(dt);
			}
		}
	}

	void Engine::OnResize(const i32 width, const i32 height)
	{
		if (m_renderAPI)
		{
			RYU_ENGINE_INFO("Resizing render API to {}x{}", width, height);
			m_renderAPI->OnResize(width, height);
		}
	}

	void Engine::Shutdown()
	{
		RYU_ENGINE_DEBUG("Shutting down engine");

		DestroyPlugins();
		m_application->OnShutdown();
		m_application.reset();

		if (m_renderAPI)
		{
			RYU_ENGINE_INFO("Shutting down renderer");
			m_renderAPI->Shutdown();
		}
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

			plugin.DLL.UnloadDLL();
		}

		RYU_ENGINE_INFO("Finished unloading {} plugins", count);
	}

	void Engine::LoadConfig()
	{
		RYU_ENGINE_DEBUG("Loading engine configuration");
		using json = nlohmann::json;

		// We do not validate EngineConfig.json since that will be validated
		// by xmake at build time during configuration

		const std::string_view engineConfig = StringFromBytes(g_engineConfig, sizeof(g_engineConfig));

		json j = json::parse(engineConfig.data());
		const auto& pluginsConfig = j["PluginsConfig"];
		const auto& plugins = pluginsConfig["Plugins"];

		for (auto& plugin : plugins)
		{
			AddPlugins(plugin);
		}
	}

	void Engine::LoadPlugins()
	{
		PluginManager::PluginMap& map = m_pluginManager.GetPluginsMap();

		for (auto& [name, plugin] : map)
		{
			// Load dll
			if (plugin.DLL.LoadDLL(plugin.PluginPath))
			{
				RYU_ENGINE_TRACE("Loaded plugin DLL: {}", name);
			}
			else
			{
				RYU_ENGINE_ERROR("Failed to load plugin DLL: {}", name);
			}

			// Register plugin
			if (RegisterPlugin_f registerPlugin = plugin.DLL.GetProcAddressFunc<RegisterPlugin_f>("RegisterPlugin"))
			{
				registerPlugin();
				RYU_ENGINE_INFO("Registered plugin: {}", name);
			}
			else
			{
				RYU_ENGINE_ERROR("Failed to register plugin: {}", name);
			}

			// Create plugin
			if (CreatePlugin_f createPlugin = plugin.DLL.GetProcAddressFunc<CreatePlugin_f>("CreatePlugin"))
			{
				plugin.Plugin = createPlugin();
				RYU_ENGINE_DEBUG("Created plugin: {}", name);
			}
			else
			{
				RYU_ENGINE_ERROR("Failed to create plugin: {}", name);
			}

			// Get destroy plugin function
			if (DestroyPlugin_f destroyPlugin = plugin.DLL.GetProcAddressFunc<DestroyPlugin_f>("DestroyPlugin"))
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
			.App = m_application.get(),
		};

		PluginManager::PluginMap& map = m_pluginManager.GetPluginsMap();
		for (auto& [name, plugin] : map)
		{
			if (plugin.Plugin && plugin.Plugin->GetPluginData().LoadOrder == order)
			{
				if (!plugin.Plugin->Initialize(api))
				{
					RYU_ENGINE_FATAL("Plugin initialization failed for: {}", name);
				}
			}
		}
	}
}
