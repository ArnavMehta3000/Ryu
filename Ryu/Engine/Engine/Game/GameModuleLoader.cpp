#include "Engine/Game/GameModuleLoader.h"
#include "App/Application.h"
#include "Logger/Logger.h"
#include <Windows.h>

namespace Ryu::Engine
{
	RYU_LOG_DECLARE_CATEGORY(GameModule);
	
	GameModuleLoader::~GameModuleLoader()
	{
		UnloadModule();
	}
	
	bool GameModuleLoader::LoadModule(const std::string& dllPath)
	{
		if (m_dllHandle)
		{
			UnloadModule();
		}
		
		// --- Load DLL ---
		m_dllHandle = ::LoadLibraryA(dllPath.c_str());
		if (!m_dllHandle)
		{
			RYU_LOG_ERROR(RYU_LOG_USE_CATEGORY(GameModule), "Failed to load game module: {}", dllPath);
			return false;
		}

		// --- Find CreateGameModule function ---
		const auto createFunc = reinterpret_cast<CreateGameModuleFunc>(::GetProcAddress(static_cast<HMODULE>(m_dllHandle), "CreateGameModule"));
		if (!createFunc)
		{
			RYU_LOG_ERROR(RYU_LOG_USE_CATEGORY(GameModule), "Failed to find CreateGameModule in {}", dllPath);
			UnloadModule();
			return false;
		}

		// --- Create game module ---
		m_gameModule = createFunc();
		if (!m_gameModule)
		{
			RYU_LOG_ERROR(RYU_LOG_USE_CATEGORY(GameModule), "CreateGameModule returned nullptr in {}", dllPath);
			UnloadModule();
			return false;
		}

		RYU_LOG_INFO(RYU_LOG_USE_CATEGORY(GameModule), "Loaded game module: {}", m_gameModule->GetName());

		return true;
	}
	
	void GameModuleLoader::UnloadModule()
	{
		// The engine should be the application owner

		if (m_gameModule)
		{
			delete m_gameModule;
			m_gameModule = nullptr;
		}
		

		if (m_dllHandle)
		{
			::FreeLibrary(static_cast<HMODULE>(m_dllHandle));
			m_dllHandle = nullptr;
		}
	}
}
