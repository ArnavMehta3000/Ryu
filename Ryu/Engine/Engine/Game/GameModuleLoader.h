#pragma once
#include "Engine/Game/IGameModule.h"
#include "Memory/Ref.h"
#include <string>

namespace Ryu::Engine
{
	class RYU_API GameModuleLoader
	{
	public:
		GameModuleLoader() = default;
		~GameModuleLoader();

		bool LoadModule(const std::string& dllPath);
		void UnloadModule();

		inline NODISCARD IGameModule* GetGameModule() const { return m_gameModule; }
		inline NODISCARD bool IsModuleLoaded() const { return m_gameModule != nullptr; }
		
		NODISCARD Memory::Ref<App::Application> GetApplication();

	private:
		void* m_dllHandle = nullptr;
		IGameModule* m_gameModule = nullptr;
		Memory::Ref<App::Application> m_application;
	};
}
