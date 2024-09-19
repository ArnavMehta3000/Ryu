#pragma once
#include <Core/Includes.h>
#include <Engine/Core/Application.h>
#include <Core/Plugin/PluginManager.h>
#include <memory>

namespace Ryu
{	
	class RYU_API Engine final
	{
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

		inline Application* GetApplication() const { return m_application.get(); }

		void Run();

	private:
		Engine();
		bool PreInit();
		bool Init();
		bool PostInit();
		void AddInputCallbacks();
		void Tick(const f32 dt);
		void TickPlugins(PluginTickOrder order, const f32 dt);
		void RenderPlugins(PluginRenderOrder order, const f32 dt);
		void LoadConfig();
		void AddPlugins(const std::string& name);
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
