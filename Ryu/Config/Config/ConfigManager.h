#pragma once
#include "Common/ObjectMacros.h"
#include "Utils/Singleton.h"
#include "Elos/Event/Signal.h"
#include <filesystem>
#include <vector>


namespace Ryu::Config
{
	namespace fs = std::filesystem;

	class ConfigBase;

	class ConfigManager : public Utils::Singleton<ConfigManager>
	{
		RYU_SINGLETON_DECLARE(ConfigManager);
	public:
		void Initialize(const fs::path& configDir);
		void RegisterConfig(ConfigBase* config);
		void UnregisterConfig(ConfigBase* config);
		void SaveAll();

		NODISCARD const fs::path& GetConfigDir() const { return m_configDir; }
		Elos::Signal<bool> OnInitialized;

	private:
		ConfigManager() = default;
		~ConfigManager() = default;

		std::filesystem::path m_configDir;
		std::vector<ConfigBase*> m_registeredConfigs;
	};
}
