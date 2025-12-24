#pragma  once
#include "Core/Utils/Singleton.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace Ryu::App
{
	class PathManager : public Utils::Singleton<PathManager>
	{
		RYU_SINGLETON_DECLARE(PathManager);

	public:
		[[nodiscard]]  inline const fs::path& GetProjectDir() const { return m_projectDir; }
		[[nodiscard]]  inline const fs::path& GetConfigDir() const { return m_configDir; }

	private:
		PathManager();

	private:
		fs::path m_projectDir;
		fs::path m_configDir;
	};
}
