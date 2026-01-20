#pragma once
#include "Engine/HotReload/GameModuleABI.h"
#include "Engine/Services/Services.h"
#include "Core/Logging/Logger.h"
#include <spdlog/fmt/fmt.h>

namespace Ryu::Game
{
	RYU_TODO("Add other wrapers around the services pointers")
	class GameServices
	{
	public:
		explicit GameServices(RyuServices services);

		template<typename... Args>
		void LogTrace(std::string_view category, fmt::format_string<Args...> format, Args&&... args)
		{
			auto* svc = GetEngineServices();
			if (svc && svc->Logger)
			{
				svc->Logger->Trace(category, format, std::forward<Args>(args)...);
			}
		}

		template<typename... Args>
		void LogDebug(std::string_view category, fmt::format_string<Args...> format, Args&&... args)
		{
			auto* svc = GetEngineServices();
			if (svc && svc->Logger)
			{
				svc->Logger->Debug(category, format, std::forward<Args>(args)...);
			}
		}

		template<typename... Args>
		void LogInfo(std::string_view category, fmt::format_string<Args...> format, Args&&... args)
		{
			auto* svc = GetEngineServices();
			if (svc && svc->Logger)
			{
				svc->Logger->Info(category, format, std::forward<Args>(args)...);
			}
		}

		template<typename... Args>
		void LogWarn(std::string_view category, fmt::format_string<Args...> format, Args&&... args)
		{
			auto* svc = GetEngineServices();
			if (svc && svc->Logger)
			{
				svc->Logger->Warn(category, format, std::forward<Args>(args)...);
			}
		}

		template<typename... Args>
		void LogError(std::string_view category, fmt::format_string<Args...> format, Args&&... args)
		{
			auto* svc = GetEngineServices();
			if (svc && svc->Logger)
			{
				svc->Logger->Error(category, format, std::forward<Args>(args)...);
			}
		}

		template<typename... Args>
		void LogFatal(std::string_view category, fmt::format_string<Args...> format, Args&&... args)
		{
			auto* svc = GetEngineServices();
			if (svc && svc->Logger)
			{
				svc->Logger->Fatal(category, format, std::forward<Args>(args)...);
			}
		}

		Logging::Logger* GetLogger() const;
		Config::ConfigManager* GetConfigManager() const;
		Config::CmdLine* GetCmdLine() const;
		App::PathManager* GetPathManager() const;
		Gfx::Renderer* GetRenderer() const;
		Gfx::ShaderCompiler* GetShaderCompiler() const;
		Game::InputManager* GetInputManager() const;
		App::IApplication* GetApplication() const;

	private:
		inline Engine::Services* GetEngineServices() const { return static_cast<Engine::Services*>(m_services); }

	private:
		RyuServices m_services;
	};

	namespace Internal
	{
		inline thread_local GameServices* g_services = nullptr;
	}
}

#define RYU_GAME_LOG_CATEGORY_NAME "Game"

#define _RYU_GAME_LOG_IMPL(Level, ...)                                                                                           \
do                                                                                                                               \
{                                                                                                                                \
	if constexpr (static_cast<i32>(::Ryu::Logging::LogLevel::Level) >= static_cast<i32>(::Ryu::Logging::COMPILE_TIME_LOG_LEVEL)) \
	{                                                                                                                            \
	    /* The reason why we don't use the `RYU_FILE` macro here is becuase of the DLL boundary issues in spdlog */              \
	    /* that are caused when a new logger is created by the filename/category but the module has been unloaded */             \
		::Ryu::Game::Internal::g_services->Log##Level(RYU_GAME_LOG_CATEGORY_NAME, __VA_ARGS__);                                  \
	}                                                                                                                            \
} while(0)

#define RYU_GAME_LOG_TRACE(...) _RYU_GAME_LOG_IMPL(Trace, __VA_ARGS__)
#define RYU_GAME_LOG_DEBUG(...) _RYU_GAME_LOG_IMPL(Debug, __VA_ARGS__)
#define RYU_GAME_LOG_INFO(...) _RYU_GAME_LOG_IMPL(Info, __VA_ARGS__)
#define RYU_GAME_LOG_WARN(...) _RYU_GAME_LOG_IMPL(Warn, __VA_ARGS__)

#define RYU_GAME_LOG_ERROR(...) do { ::Ryu::Game::Internal::g_services->LogError(RYU_FILE, __VA_ARGS__);  } while(0)
#define RYU_GAME_LOG_FATAL(...) do { ::Ryu::Game::Internal::g_services->LogFatal(RYU_FILE, __VA_ARGS__);  } while(0)

