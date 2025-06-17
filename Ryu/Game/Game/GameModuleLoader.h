#pragma once
#include "Game/IGameModule.h"
#include <string>

namespace Ryu::Game
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

	private:
		void*        m_dllHandle = nullptr;
		IGameModule* m_gameModule = nullptr;
	};
}
