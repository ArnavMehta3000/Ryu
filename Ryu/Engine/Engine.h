#pragma once
#include <Core/Includes.h>
#include <Engine/Core/Application.h>
#include <Core/Plugin/PluginManager.h>
#include <memory>
#include <initializer_list>

namespace Ryu
{	
	class RYU_API Engine final
	{
	public:
		static constexpr std::string_view ENGINE_PLUGINS_PATH = "Plugins/Engine/";
	public:
		~Engine();

		static Engine& Get()
		{
			static Engine instance;
			return instance;
		}

		template <typename T, typename... Args>
		void CreateApplication(Args&&... args)
		{
			if (m_application)
			{
				throw std::runtime_error("Application already exists");
			}

			m_application = std::make_unique<T>(std::forward<Args>(args)...);
		}

		void AddPlugins(std::initializer_list<std::string> plugins);
		void Run();

	private:
		Engine();
		bool PreInit();
		bool Init();
		bool PostInit();
		void Tick(MAYBE_UNUSED const f32 dt);
		void LoadPlugins();
		void InitializePlugins(Ryu::PluginLoadOrder order);
		void DestroyPlugins();
		void Shutdown();

	private:
		std::unique_ptr<Application> m_application;
		PluginManager m_pluginManager;
	};

	RYU_API_FUNC(Engine&, GetEngineInstance);
}